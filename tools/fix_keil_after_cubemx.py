# -*- coding: utf-8 -*-
"""
fix_keil_after_cubemx.py
========================
用途：每次用 STM32CubeMX 重新生成 MDK-ARM 工程后，跑一遍这个脚本，
      自动修掉 CubeMX 生成文件里的 4 个固定坑，避免 Keil 打不开 / 编译报错。

修的 4 个坑：
  坑 1  编译器版本：CubeMX 默认写 AC5（ARMCC V5），但 Keil 5.37+ 已删除 AC5，
        只有 AC6（armclang）。-> 自动补上 pCCUsed + uAC6，指向 AC6.
  坑 2  畸形文件路径：CubeMX 在 Project Location 与 Toolchain Folder Location
        不一致时，会把路径写成 "../D:/xxx/Core/Src/main.c" 这种拼接体，
        Keil 解析后找不到文件。-> 自动改回 "../Core/Src/main.c".
  坑 3  重复文件组：同一个原因会让 Core 下的 .c 文件被加进两个 Group，
        编译出 main.o 和 main_1.o，链接时报 L6200E multiply defined.
        -> 自动删除组名里带盘符路径的那个重复组。
  坑 4  自己写的源码被挤掉：CubeMX 重新生成时会按模板重写整个 <Groups>，
        src/ 下的 bsp_*.c 和包含路径 ../src 都会丢，编译报 "cannot open
        source input file" / "bsp_led.h: No such file".
        -> 自动扫描 src/*.c 重建 BSP 组，并把 ../src 加进 IncludePath。

用法（在任意目录都行）：
    python tools/fix_keil_after_cubemx.py
    python tools/fix_keil_after_cubemx.py 指定工程.uvprojx
"""

import re
import sys
import glob
import shutil
from pathlib import Path

# 让中文在 Windows 命令行（GBK 代码页）里正常显示，避免乱码
if sys.platform == "win32":
    try:
        sys.stdout.reconfigure(encoding="gbk", errors="replace")
    except Exception:
        pass

# Keil MDK 5.40 自带的 AC6 版本标识：6220000::V6.22::ARMCLANG
AC6_TAG = "6220000::V6.22::ARMCLANG"

# 自己写的业务代码在工程里的分组名（src/*.c 都放这个组）
BSP_GROUP_NAME = "Application/User/BSP"


def find_project_file() -> Path:
    """自动定位 MDK-ARM 目录下的 .uvprojx 工程文件。"""
    # 脚本位于 <仓库根>/tools/ 下，所以仓库根是上一级
    repo_root = Path(__file__).resolve().parent.parent
    hits = sorted(glob.glob(str(repo_root / "MDK-ARM" / "*.uvprojx")))
    if not hits:
        print("× 没找到 MDK-ARM/*.uvprojx，请把脚本放在仓库的 tools/ 目录下，")
        print("  或者直接在命令行把工程文件路径作为参数传进来。")
        sys.exit(1)
    if len(hits) > 1:
        print("! 找到多个工程文件，使用第一个：")
        for h in hits:
            print("   ", h)
    return Path(hits[0])


def fix_compiler(text: str) -> tuple[str, str]:
    """坑 1：确保工程声明使用 AC6 编译器。"""
    if "<uAC6>1</uAC6>" in text:
        return text, "已在使用 AC6，跳过"

    # 情况 A：已经有 pCCUsed，但指向 AC5 -> 整体替换
    m = re.search(r"<pCCUsed>([^<]*)</pCCUsed>", text)
    if m:
        old = m.group(1)
        text = re.sub(r"<pCCUsed>[^<]*</pCCUsed>",
                      f"<pCCUsed>{AC6_TAG}</pCCUsed>", text)
        if "<uAC6>" in text:
            text = re.sub(r"<uAC6>\d</uAC6>", "<uAC6>1</uAC6>", text)
        else:
            text = text.replace(f"<pCCUsed>{AC6_TAG}</pCCUsed>",
                                f"<pCCUsed>{AC6_TAG}</pCCUsed>\n      <uAC6>1</uAC6>")
        return text, f"pCCUsed: {old} -> {AC6_TAG}"

    # 情况 B：完全没有 pCCUsed（CubeMX 的常见输出）-> 插在 ToolsetName 后面
    anchor = "<ToolsetName>ARM-ADS</ToolsetName>"
    if anchor not in text:
        return text, "× 没找到 ToolsetName 锚点，未修改（请手动在 Keil 里选 V6.22）"

    new_block = (anchor + "\n      <pCCUsed>" + AC6_TAG +
                 "</pCCUsed>\n      <uAC6>1</uAC6>")
    # 只替换第一个（多 Target 时只处理主 Target）
    text = text.replace(anchor, new_block, 1)
    return text, f"补上 pCCUsed = {AC6_TAG}"


def fix_paths(text: str) -> tuple[str, str]:
    """坑 2：把 "../D:/xxx/Core/Src/main.c" 改回 "../Core/Src/main.c"。"""
    # 匹配 ../ 后面跟盘符冒号，一直到 /Core/ 或 /Drivers/
    pattern = re.compile(r"\.\./[A-Za-z]:[^<]*?/(Core|Drivers)/")
    hits = pattern.findall(text)
    if not hits:
        return text, "路径正常，跳过"
    text = pattern.sub(r"../\1/", text)
    return text, f"修正 {len(hits)} 处畸形路径"


def fix_duplicate_groups(text: str) -> tuple[str, str]:
    """坑 3：删除组名里嵌了盘符路径的重复 Group。"""
    removed = []

    def kill(match):
        name = match.group(1)
        # 组名里出现 "D:" / "C:/" 这类盘符 -> 判为 CubeMX 重复生成的组
        if re.search(r"[A-Za-z]:[/\\]", name):
            removed.append(name)
            return ""
        return match.group(0)

    # 注意 <Group> 内没有再嵌套 <Group>，非贪婪匹配安全
    group_pat = re.compile(
        r"[ \t]*<Group>\s*<GroupName>([^<]*)</GroupName>.*?</Group>", re.S)
    text = group_pat.sub(kill, text)

    if not removed:
        return text, "无重复组，跳过"
    return text, "删除重复组: " + ", ".join(removed)


def ensure_bsp_sources(text: str, repo_root: Path) -> tuple[str, str]:
    """坑 4：CubeMX 重新生成时会把整个 <Groups> 按模板重写，
    我们自己加的 BSP 组（src/*.c）和包含路径 ../src 会被抹掉。
    这里扫描 src/ 目录自动补回来，幂等，跑几次结果都一样。"""
    src_dir = repo_root / "src"
    c_files = sorted(p.name for p in src_dir.glob("*.c")) if src_dir.is_dir() else []
    if not c_files:
        return text, "src/ 下没有 .c 文件，跳过"

    # ---- 1. 包含路径补上 ../src ----
    inc_note = ""
    m = re.search(r"<IncludePath>([^<]*)</IncludePath>", text)
    if not m:
        return text, "× 没找到 IncludePath，跳过"
    inc = m.group(1)
    if "../src" not in inc.split(";"):
        text = text.replace(f"<IncludePath>{inc}</IncludePath>",
                            f"<IncludePath>../src;{inc}</IncludePath>", 1)
        inc_note = "已加入 ../src"

    # ---- 2. 生成 BSP 组 ----
    files_xml = "\n".join(
        "            <File>\n"
        f"              <FileName>{name}</FileName>\n"
        "              <FileType>1</FileType>\n"
        f"              <FilePath>../src/{name}</FilePath>\n"
        "            </File>" for name in c_files)

    group_xml = ("        <Group>\n"
                 f"          <GroupName>{BSP_GROUP_NAME}</GroupName>\n"
                 "          <Files>\n"
                 f"{files_xml}\n"
                 "          </Files>\n"
                 "        </Group>\n")

    group_pat = re.compile(
        r"[ \t]*<Group>\s*<GroupName>" + re.escape(BSP_GROUP_NAME) +
        r"</GroupName>.*?</Group>(\s*)", re.S)

    if group_pat.search(text):
        text = group_pat.sub(lambda _m: group_xml, text, count=1)
        msg = f"同步 BSP 组（{len(c_files)} 个文件）"
    else:
        anchor = "<Group>\n          <GroupName>Drivers/STM32F1xx_HAL_Driver</GroupName>"
        if anchor not in text:
            return text, "× 没找到 Drivers 组锚点，未加 BSP 组"
        text = text.replace(anchor, group_xml + anchor, 1)
        msg = f"新建 BSP 组（{len(c_files)} 个文件）"

    if inc_note:
        msg += "，" + inc_note
    return text, msg


def main():
    prj = Path(sys.argv[1]) if len(sys.argv) > 1 else find_project_file()
    print(f"工程文件: {prj}")
    if not prj.exists():
        print("× 文件不存在")
        sys.exit(1)

    raw = prj.read_bytes()
    # 保留原始换行风格（Keil 生成的是 CRLF）
    crlf = b"\r\n" in raw
    text = raw.decode("utf-8-sig")
    text = text.replace("\r\n", "\n")

    backup = prj.with_suffix(".uvprojx.bak")
    shutil.copy2(prj, backup)
    print(f"已备份 -> {backup.name}")

    text, msg1 = fix_compiler(text)
    text, msg2 = fix_paths(text)
    text, msg3 = fix_duplicate_groups(text)
    text, msg4 = ensure_bsp_sources(text, prj.resolve().parent.parent)

    # XML 收尾：清理可能出现的连续空行
    text = re.sub(r"\n[ \t]*\n[ \t]*\n+", "\n", text)

    out = text.replace("\n", "\r\n") if crlf else text
    prj.write_bytes(out.encode("utf-8"))

    print("\n修复结果：")
    print("  [1] 编译器  ->", msg1)
    print("  [2] 文件路径 ->", msg2)
    print("  [3] 重复文件组 ->", msg3)
    print("  [4] BSP 源码组 ->", msg4)
    print("\n完成。现在用 Keil 打开工程按 F7，或者在命令行执行：")
    print('  "D:\\STM32_TOOLS\\keil MDK\\CORE\\UV4\\UV4.exe" -r '
          f'"{prj}" -j0 -o "{prj.parent / "build.log"}"')


if __name__ == "__main__":
    main()

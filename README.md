# 2026 DEBUG 实验室软件组招新赛题 · 循迹小车

浙大城市学院信息与电气工程学院 DEBUG 实验室 2026 年招新考核赛题实现。

- 姓名：白鑫（Bai Xin）
- 学号：32602390
- 班级：自动化 2604
- 主控：STM32F103C8T6
- 开发环境：Keil MDK / STM32CubeMX

---

## 一、项目结构

```
line_follow_2026/
├── README.md                   本文件
├── .gitignore                  忽略编译产物
├── line_follow_2026.ioc        CubeMX 工程配置（改引脚/时钟靠它）
├── Core/
│   ├── Inc/                    main.h / stm32f1xx_hal_conf.h / stm32f1xx_it.h
│   └── Src/                    main.c / stm32f1xx_it.c / stm32f1xx_hal_msp.c
├── Drivers/                    STM32F1 HAL 库 + CMSIS（勿手改）
├── MDK-ARM/
│   ├── line_follow_2026.uvprojx  Keil 工程
│   └── startup_stm32f103xb.s     启动文件
├── tools/
│   └── fix_keil_after_cubemx.py  CubeMX 重新生成后的一键修复脚本
└── README.md                   本文件
```

---

## 二、赛题进度

### 基础部分（60 分）

| 项 | 内容 | 分值 | 状态 |
|---|---|---|---|
| 1a | OLED 主菜单（≥4 项，按键 1/2 移箭头） | 3 | ⬜ |
| 1b | 子菜单（按键 3 进入 / 按键 4 返回） | 3 | ⬜ |
| 2a | LED 常亮 / 常灭（按键 1 切换） | 2 | ⬜ |
| 2b | LED 交替闪烁（3 轮 × 30 秒） | 2 | ⬜ |
| 2c | 按键 4 返回主菜单后 LED 常灭 | 1 | ⬜ |
| 2d | OLED 显示 LED 状态（1 秒内更新） | 1 | ⬜ |
| 3a | 信息显示（姓名拼音 / 学号 / 变量 a） | 2 | ⬜ |
| 3b | 上位机修改变量 a（串口） | 3 | ⬜ |
| 3c | 上位机读取变量 a（GET A，支持负数） | 3 | ⬜ |
| 4a | OLED 显示全部巡线传感器状态 | 2 | ⬜ |
| 4b | 正常循迹一圈（含自动计时显示） | 8 | ⬜ |
| 4c | 连续循迹两圈（第一圈不停车） | 4 | ⬜ |
| 4d | 特殊元素（Ω弯 / 圆环 / 十字 / 直角 / 断路 / 折线） | 10 | ⬜ |
| 4e | 完整复杂地图（3 个检查点，5 次机会） | 11 | ⬜ |
| 5 | Git 仓库与开发记录 | 5 | ⬜ |

### 拓展部分（40 分，基础满分后统计）

| 项 | 内容 | 分值 | 状态 |
|---|---|---|---|
| 6.1 | 随机地图循迹 | 15 | ⬜ |
| 6.2 | 参数设置与掉电保存 | 5 | ⬜ |
| 6.3 | 遥控小车 | 10 | ⬜ |
| 6.4 | 巡线速度与控制性能 | 10 | ⬜ |

> 图例：⬜ 未开始 ｜ 🟨 进行中 ｜ ✅ 已完成 ｜ ❌ 有问题

---

## 三、硬件配置

| 模块 | 型号 | 接线 |
|---|---|---|
| 主控 | STM32F103C8T6 | — |
| 下载器 | ST-LINK V2 | SWDIO / SWCLK / GND / 3.3V |
| 显示 | OLED 0.96" I2C | SCL / SDA |
| 按键 | 6×6 轻触 ×4 | 按键 1~4 |
| 指示 | LED ×2 | — |
| 蜂鸣器 | 有源，高电平触发 | VCC 5V / IN GPIO / 共地 |
| 电机驱动 | TB6612FNG | — |
| 巡线 | TCRT5000 五路（数字输出） | — |

### 引脚分配表

> 全部定义集中在 `src/bsp_config.h`，**接线和这张表不一致时只改那一个文件**，
> 其它 `.c` 一行都不用动。

| 模块 | 信号 | 引脚 | 模式 |
|---|---|---|---|
| LED1 | LED1 | PB12 | 推挽输出 |
| LED2 | LED2 | PB13 | 推挽输出 |
| 蜂鸣器 | BUZZER | PB14 | 推挽输出 |
| 按键 1 | KEY1 | PA0 | 输入 + 内部上拉 |
| 按键 2 | KEY2 | PA1 | 输入 + 内部上拉 |
| 按键 3 | KEY3 | PA2 | 输入 + 内部上拉 |
| 按键 4 | KEY4 | PA3 | 输入 + 内部上拉 |
| 巡线 1~5 | S1~S5 | PA4 / PA5 / PB0 / PB1 / PB15 | 输入（待硬件核对） |
| 电机 PWMA | PWMA | PA6 (TIM3_CH1) | 复用·PWM |
| 电机 PWMB | PWMB | PA7 (TIM3_CH2) | 复用·PWM |
| 电机 AIN1/AIN2 | AIN1/AIN2 | PB8 / PB9 | 推挽输出 |
| 电机 BIN1/BIN2 | BIN1/BIN2 | PA11 / PA12 | 推挽输出 |
| OLED | SCL / SDA | PB6 / PB7 | I2C1 |
| 串口 | TX / RX | PA9 / PA10 | USART1 · 115200 8N1 |

---

## 四、开发约定

1. **非阻塞**：OLED 刷新、串口发送、蜂鸣器都必须非阻塞，禁止在循线循环里用 `HAL_Delay`
2. **供电分离**：电机走电池电压，主控走降压后 5V/3.3V，必须共地
3. **提交纪律**：赛题要求「每周至少一次有效提交」，每次完成一个可验证的小功能就 commit
4. **提交信息格式**：`[模块] 做了什么`
   - 例：`[oled] 实现主菜单显示与按键移箭头`
   - 例：`[uart] 支持 GET A 查询，修复负数解析`

---

## 五、坑位记录

遇到并解决的问题记录在这里，面试代码答辩时会用到。

| 日期 | 问题 | 原因 | 解决 |
|---|---|---|---|
| 10-25 | Keil 编译报 `uses ARM-Compiler 'Default Compiler Version 5' which is not available`，直接 Build aborted | CubeMX 生成的 `.uvprojx` 里没有 `<pCCUsed>` 字段，Keil 兜底去找 AC5；而 MDK 从 5.37 起已彻底移除 AC5，本机只有 AC6（armclang 6.22） | 在 `<Target>` 下补 `<pCCUsed>6220000::V6.22::ARMCLANG</pCCUsed>` 与 `<uAC6>1</uAC6>` |
| 10-25 | `armclang: error: no such file or directory: '../D:/Debug/line_follow_2026/Core/Src/main.c'` | 生成时 `Project Location=D:/Debug` 与 `Toolchain Folder Location=D:\Debug\line_follow_2026\` 不一致，CubeMX 写出「相对前缀 + 绝对路径」的拼接路径，Keil 从 MDK-ARM 解析后找不到 | 统一改回 `../Core/Src/xxx.c` |
| 10-25 | 链接报 `L6200E: Symbol main multiply defined (by main_1.o and main.o)` 等 24 个重复定义 | 同一个路径错位问题，导致 `main.c / stm32f1xx_it.c / stm32f1xx_hal_msp.c` 被加进两个 Group（第二个组名是 `Application/User/D:/Debug/line_follow_2026/Core`），编译出两份 .o | 删除组名里嵌了盘符路径的重复 Group |
| 10-25 | CubeMX 点 Generate Code 后长时间无反应，日志停在 `Download File stm32cube_fw_f1_v187.zip` | 生成前会校验固件包完整性，缺什么就联网补下（110MB 量级），不是死机 | 等待下载与解压完成（本次约 8 分钟），不要点关闭 |
| 10-25 | 按键引脚 CubeMX 里配的是 `GPIO_NOPULL`，按键一端接 GND 时会悬空乱跳，读出来的值完全随机 | 轻触按键只是个开关，不按下时引脚必须由电阻拉到确定电平；内部上下拉没开就等于天线 | `bsp_key.c` 的 `KEY_Init()` 里按实际接线重新 `HAL_GPIO_Init()` 一遍补上内部上拉（`KEY_USE_INTERNAL_PULLUP`），不去改 CubeMX 配置，重新生成也不会丢 |
| 10-25 | 把业务代码放 `src/` 后，CubeMX 一重新生成，Keil 里 BSP 组和 `../src` 包含路径全没了，编译报找不到 `bsp_led.h` | CubeMX 生成时会按模板把整个 `<Groups>` 重写，只认识自己生成的文件 | 给 `fix_keil_after_cubemx.py` 加第 4 项：扫描 `src/*.c` 自动重建 BSP 组 + 补 `../src` 包含路径，幂等 |

> 以上前三个坑可以用一条命令一次性修好，不必手改 XML：
>
> ```bash
> python tools/fix_keil_after_cubemx.py
> ```
>
> 脚本会自动补 AC6 标识、修正畸形路径、删掉重复文件组，并保留 `.bak` 备份。
> 每次用 CubeMX 重新生成代码后跑一遍即可。

---

## 六、环境速查

| 项目 | 路径 / 版本 |
|---|---|
| Keil MDK | `D:\STM32_TOOLS\keil MDK\`（MDK Plus 5.40） |
| ARM 编译器 | Arm Compiler for Embedded 6.22（AC6，不含 AC5） |
| 器件包 | `D:\STM32_TOOLS\keil MDK\PACK\Keil\STM32F1xx_DFP\2.4.1` |
| CubeMX 固件包 | `C:\Users\KSCIN\STM32Cube\Repository\STM32Cube_FW_F1_V1.8.7` |
| 命令行编译 | `UV4.exe -r 工程.uvprojx -j0 -o build.log`（退出码 0=无错无警告） |


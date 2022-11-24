# USB电压电流表

## 简介

立创EDA训练营电压电流表项目代码

* 版本：v1.0.1
* 项目主页：TBD
* 最小硬件版本：v1.0


## 编译参数

* 编译器：ARM Compiler version 5 (-O0)
* MDK版本：5.31
* 调试器：ST-Link V2


## 注意事项

如果采样得到的电压或电流不准，可以根据实际值修改`main.c`文件中`VOLTAGE_FACTOR`和`CURRENT_FACTOR`的值，具体计算公式为：

```
    VOLTAGE_FACTOR = 实际电压 / 显示电压
    CURRENT_FACTOR = 实际电流 / 显示电流
```

## 其他

### 代码作者
micespring
* https://oshwhub.com/micespring

### 引用的开源项目
Adafruit SSD1306
* https://github.com/adafruit/Adafruit_SSD1306
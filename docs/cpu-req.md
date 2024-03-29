# 仿真环境对处理器接口的要求

## 基本信号

| 接口名 | 接口名含义     | 处理器端接口类型（位宽） |
| ------ | -------------- | ------------------------ |
| clk    | 处理器全局时钟 | input（1）               |
| rstn   | 低电平复位使能 | input（1）               |

## 取指阶段

| 接口名  | 接口名含义               | 处理器端接口类型（位宽） |
| ------- | ------------------------ | ------------------------ |
| inst_IF | 仿真环境提供的指令码     | input（32）              |
| pc_IF   | IF段PC，用于提供指令地址 | output（32）             |

## 译码阶段

**无**

## 执行阶段

**无**

## 访存阶段

| 接口名        | 接口名含义                | 处理器端接口类型（位宽） |
| ------------- | ------------------------- | ------------------------ |
| mem_addr_MEM  | MEM段访存地址             | output（32）             |
| is_store_MEM  | MEM段存储器写使能         | output（1）              |
| is_load_MEM   | MEM段存储器读使能         | output（1）              |
| mem_size_MEM  | MEM段存储器读写宽度的对数 | output（2）              |
| mem_rdata_MEM | MEM段仿真环境返回的读数据 | input（32）              |
| mem_wdata_MEM | MEM段存储器写数据         | output（32）             |

## 写回阶段

| 接口名      | 接口名含义                           | 处理器端接口类型（位宽） |
| ----------- | ------------------------------------ | ------------------------ |
| rd_WB       | WB段寄存器堆写地址                   | output（5）              |
| rf_we_WB    | WB段寄存器堆写使能                   | output（1）              |
| rf_wdata_WB | WB段寄存器堆写数据                   | output（32）             |
| inst_WB     | WB段指令码                           | output（32）             |
| pc_WB       | WB段pc                               | output（32）             |
| commit_WB   | WB段提交使能（表示当前指令不是气泡） | output（1）              |
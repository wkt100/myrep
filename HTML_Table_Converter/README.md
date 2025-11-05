# HTML表格转CSV转换工具

## 功能介绍

本工具用于将HTML文件中的表格数据批量转换为CSV格式，支持复杂的表格结构解析。主要特点：

- 无需外部依赖，仅使用Python标准库
- 支持目录递归处理，保留原始文件结构
- 命令行参数化，支持自定义输入输出路径
- 适用于各类HTML报表、文档中的表格数据提取
- 生成的CSV文件可直接用于Excel打开或数据库导入

## 系统要求

- Python 3.6 或更高版本
- 无需安装额外Python库，仅使用标准库中的`html.parser`和`argparse`

## 安装方法

1. 将整个`HTML_Table_Converter`文件夹复制到您的工作目录
2. 确保您的系统已安装Python 3.6或更高版本
3. 无需安装额外依赖

## 使用方法

### 基本使用

在命令行中运行以下命令，使用默认参数：

```bash
python html_tables_to_csv.py
```

默认情况下，工具会在当前工作目录下查找`reports`文件夹作为输入，并将结果输出到`converted_csv`文件夹。

### 自定义参数

使用`-r`或`--report-dir`参数指定HTML报告目录：

```bash
python html_tables_to_csv.py --report-dir "路径/to/your/reports"
```

使用`-o`或`--output-dir`参数指定CSV输出目录：

```bash
python html_tables_to_csv.py --output-dir "路径/to/output"
```

同时使用两个参数：

```bash
python html_tables_to_csv.py --report-dir "输入目录" --output-dir "输出目录"
```

查看帮助信息：

```bash
python html_tables_to_csv.py --help
```

## 参数说明

| 参数 | 全称 | 描述 | 默认值 |
|------|------|------|--------|
| `-r` | `--report-dir` | HTML报告文件夹路径 | 当前目录下的`reports`文件夹 |
| `-o` | `--output-dir` | CSV输出文件夹路径 | 当前目录下的`converted_csv`文件夹 |
| `-h` | `--help` | 显示帮助信息 | - |

## 工具包目录结构

```
HTML_Table_Converter/
├── html_tables_to_csv.py  # 主程序脚本
├── README.md              # 使用说明书（本文档）
└── examples/              # 示例文件
    ├── general.html       # 综合信息表格示例
    ├── callbacks/         # 回调信息表格示例
    │   ├── call0.html     # 回调示例1
    │   └── call1.html     # 回调示例2
    └── widgets/           # 控件信息表格示例
        ├── ActivateButton.html  # 按钮控件示例
        └── AnalogDisplay.html   # 模拟显示控件示例
```

## 示例使用

### 使用示例文件

您可以使用`examples`目录中的示例文件来测试工具：

```bash
python html_tables_to_csv.py --report-dir examples --output-dir example_output
```

执行后，工具会将`examples`目录中的所有HTML文件转换为CSV格式，并保存到`example_output`目录。

## 输出格式说明

- 对于每个HTML文件，工具会生成一个对应的CSV文件
- 保留原始目录结构，例如：`reports/widgets/`下的HTML文件会转换到`converted_csv/widgets/`目录
- CSV文件使用UTF-8编码，确保中文等非ASCII字符能正确显示
- 表格的表头会作为CSV文件的第一行
- 支持包含多个表格的HTML文件，会将所有表格合并到一个CSV文件中

## 使用场景建议

1. **数据分析**：将HTML报表数据提取为CSV格式，便于使用Excel或数据分析工具进行进一步分析

2. **数据迁移**：将网页表格数据导出为标准CSV格式，便于导入到数据库系统

3. **自动化处理**：可集成到自动化脚本中，定期处理HTML报告

4. **界面布局分析**：对于包含控件位置信息的表格，可以分析界面布局

5. **事件流程分析**：对于包含回调信息的表格，可以分析系统事件流程

## 注意事项

1. 工具使用Python内置的`HTMLParser`进行解析，对于非常复杂或不规范的HTML可能存在兼容性问题

2. 请确保输入目录路径正确，包含有效的HTML文件

3. 输出目录会自动创建，如果目录已存在，可能会覆盖已有的同名CSV文件

4. 对于包含特殊字符的表格内容，会进行适当处理以确保CSV格式的正确性

5. 在Windows系统中，建议使用双引号包围路径参数，特别是当路径包含空格时

## 故障排除

### 常见问题

**Q: 脚本执行时提示找不到目录？**
A: 请检查`--report-dir`参数指定的路径是否存在，或使用绝对路径。

**Q: 生成的CSV文件中中文显示乱码？**
A: CSV文件使用UTF-8编码，请确保您的查看工具（如Excel）正确设置了编码格式。

**Q: 部分表格数据没有被正确解析？**
A: 工具对标准HTML表格支持较好，但对于使用非标准格式或JavaScript动态生成的表格可能解析不完整。

## 版本信息

- **版本**: 1.0
- **更新日期**: 2025年11月5日
- **开发者**: AI助手

## 许可证

本工具仅供内部使用，请勿用于商业用途。
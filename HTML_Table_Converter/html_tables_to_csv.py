#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
HTML表格转换工具
将PhotonTemplate_zh/reports目录下的HTML表格转换为CSV格式
"""

import os
import csv
import re
import argparse
from html.parser import HTMLParser

# 确保中文正常显示
import sys
sys.stdout.reconfigure(encoding='utf-8')

class TableParser(HTMLParser):
    """HTML表格解析器"""
    def __init__(self):
        super().__init__()
        self.in_table = False
        self.in_row = False
        self.in_cell = False
        self.in_header = False
        self.in_bold = False
        self.current_table = []
        self.current_row = []
        self.current_cell = []
        self.tables = []
        self.current_tag = ''
    
    def handle_starttag(self, tag, attrs):
        self.current_tag = tag
        if tag == 'table':
            self.in_table = True
            self.current_table = []
        elif self.in_table and tag == 'tr':
            self.in_row = True
            self.current_row = []
        elif self.in_row and (tag == 'td' or tag == 'th'):
            self.in_cell = True
            self.in_header = (tag == 'th')
            self.current_cell = []
        elif self.in_cell and tag == 'b':
            self.in_bold = True
    
    def handle_endtag(self, tag):
        if tag == 'table':
            self.in_table = False
            # 只添加有数据的表格
            if len(self.current_table) > 1:  # 至少有表头和一行数据
                self.tables.append(self.current_table)
        elif self.in_table and tag == 'tr':
            self.in_row = False
            if self.current_row:  # 确保行不为空
                self.current_table.append(self.current_row)
        elif self.in_cell and (tag == 'td' or tag == 'th'):
            self.in_cell = False
            self.in_header = False
            cell_text = ''.join(self.current_cell).strip()
            self.current_row.append(cell_text)
        elif tag == 'b':
            self.in_bold = False
    
    def handle_data(self, data):
        if self.in_cell:
            self.current_cell.append(data)


def extract_table_data(html_content):
    """从HTML内容中提取表格数据"""
    parser = TableParser()
    parser.feed(html_content)
    
    if not parser.tables:
        print("警告: 未在HTML中找到表格")
        return []
    
    table_data = []
    
    for table in parser.tables:
        if not table:
            continue
        
        # 表头就是第一行
        headers = table[0]
        
        # 数据行是剩余行
        rows = table[1:]
        
        # 清理数据：移除空行，确保行长度与表头一致
        cleaned_rows = []
        for row in rows:
            if row:  # 跳过空行
                # 确保行数据长度与表头一致
                if len(row) >= len(headers):
                    cleaned_rows.append(row[:len(headers)])
                elif len(row) < len(headers):
                    # 如果行数据比表头少，用空字符串补齐
                    padded_row = row + [''] * (len(headers) - len(row))
                    cleaned_rows.append(padded_row)
        
        if headers and cleaned_rows:
            table_data.append((headers, cleaned_rows))
    
    return table_data

def clean_filename(filename):
    """清理文件名，移除不适合的字符"""
    return re.sub(r'[^a-zA-Z0-9_\u4e00-\u9fa5]', '_', filename)

def process_html_file(html_path, output_dir):
    """处理单个HTML文件"""
    try:
        with open(html_path, 'r', encoding='utf-8') as f:
            html_content = f.read()
    except UnicodeDecodeError:
        try:
            with open(html_path, 'r', encoding='gbk') as f:
                html_content = f.read()
        except Exception as e:
            print(f"错误: 无法读取文件 {html_path}: {e}")
            return
    
    # 提取表格数据
    table_data_list = extract_table_data(html_content)
    
    if not table_data_list:
        print(f"警告: 文件 {html_path} 中没有找到有效表格")
        return
    
    # 获取基本文件名（不含路径）
    base_name = os.path.basename(html_path)
    file_name_without_ext = os.path.splitext(base_name)[0]
    
    # 为每个表格创建CSV文件
    for table_idx, (headers, rows) in enumerate(table_data_list):
        # 如果只有一个表格，就用原文件名；如果有多个表格，添加索引
        if len(table_data_list) == 1:
            csv_filename = f"{file_name_without_ext}.csv"
        else:
            csv_filename = f"{file_name_without_ext}_table{table_idx+1}.csv"
        
        csv_path = os.path.join(output_dir, clean_filename(csv_filename))
        
        try:
            with open(csv_path, 'w', newline='', encoding='utf-8-sig') as csvfile:
                writer = csv.writer(csvfile)
                # 写入表头
                writer.writerow(headers)
                # 写入数据行
                writer.writerows(rows)
            print(f"已转换: {html_path} -> {csv_path} (表格 {table_idx+1})")
        except Exception as e:
            print(f"错误: 无法写入CSV文件 {csv_path}: {e}")

def process_directory(reports_dir, output_dir):
    """处理整个目录下的HTML文件"""
    # 确保输出目录存在
    os.makedirs(output_dir, exist_ok=True)
    
    # 处理general.html
    general_html = os.path.join(reports_dir, 'general.html')
    if os.path.exists(general_html):
        process_html_file(general_html, output_dir)
    
    # 处理callbacks目录
    callbacks_dir = os.path.join(reports_dir, 'callbacks')
    if os.path.isdir(callbacks_dir):
        callbacks_output_dir = os.path.join(output_dir, 'callbacks')
        os.makedirs(callbacks_output_dir, exist_ok=True)
        
        for file in os.listdir(callbacks_dir):
            if file.endswith('.html'):
                html_path = os.path.join(callbacks_dir, file)
                process_html_file(html_path, callbacks_output_dir)
    
    # 处理widgets目录
    widgets_dir = os.path.join(reports_dir, 'widgets')
    if os.path.isdir(widgets_dir):
        widgets_output_dir = os.path.join(output_dir, 'widgets')
        os.makedirs(widgets_output_dir, exist_ok=True)
        
        for file in os.listdir(widgets_dir):
            if file.endswith('.html'):
                html_path = os.path.join(widgets_dir, file)
                process_html_file(html_path, widgets_output_dir)

def main():
    # 解析命令行参数
    parser = argparse.ArgumentParser(description='将HTML表格转换为CSV格式')
    parser.add_argument('-r', '--report-dir', type=str, 
                        default=os.path.join(os.getcwd(), 'PhotonTemplate_zh', 'reports'),
                        help='HTML报告目录路径')
    parser.add_argument('-o', '--output-dir', type=str, 
                        default=os.path.join(os.getcwd(), 'converted_csv'),
                        help='CSV输出目录路径')
    
    args = parser.parse_args()
    
    # 确保路径格式正确（处理反斜杠）
    report_dir = os.path.normpath(args.report_dir)
    output_dir = os.path.normpath(args.output_dir)
    
    print(f"开始处理HTML表格...")
    print(f"报告目录: {report_dir}")
    print(f"输出目录: {output_dir}")
    
    # 脚本现在使用Python内置的HTMLParser，不需要外部依赖
    
    # 开始处理
    process_directory(report_dir, output_dir)
    
    print("\n转换完成!")
    print(f"所有CSV文件已保存到: {args.output_dir}")
    print("\n提示：")
    print("1. CSV文件可以直接用Excel打开进行分析")
    print("2. 也可以导入到数据库中进行更复杂的查询和分析")
    print("3. callbacks目录包含所有回调函数的信息")
    print("4. widgets目录包含所有控件的详细信息")

if __name__ == '__main__':
    main()
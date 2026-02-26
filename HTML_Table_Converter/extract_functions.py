#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
从CSV文件中提取所有Function并去重
"""

import os
import csv
from pathlib import Path

def extract_functions_from_csvs(callbacks_dir):
    """从所有CSV文件中提取函数名"""
    functions = set()
    
    # 获取所有CSV文件
    csv_files = [f for f in os.listdir(callbacks_dir) if f.endswith('_csv')]
    
    print(f"找到 {len(csv_files)} 个CSV文件")
    
    for csv_file in csv_files:
        file_path = os.path.join(callbacks_dir, csv_file)
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                reader = csv.reader(f)
                header = next(reader)  # 跳过表头
                for row in reader:
                    if row and len(row) > 0:
                        function_name = row[0].strip()
                        if function_name:  # 只添加非空的函数名
                            functions.add(function_name)
        except Exception as e:
            print(f"处理文件 {csv_file} 时出错: {e}")
    
    return sorted(functions)

def main():
    callbacks_dir = r"C:\Users\12987\Desktop\now\DG\2025.10.21\C_Call_Analyzer\callbacks"
    output_file = r"C:\Users\12987\Desktop\now\DG\2025.10.21\C_Call_Analyzer\all_functions.txt"
    
    # 提取函数名
    functions = extract_functions_from_csvs(callbacks_dir)
    
    # 写入文件
    with open(output_file, 'w', encoding='utf-8') as f:
        for func in functions:
            f.write(func + '\n')
    
    print(f"\n总共提取到 {len(functions)} 个不同的函数名")
    print(f"结果已保存到: {output_file}")

if __name__ == "__main__":
    main()
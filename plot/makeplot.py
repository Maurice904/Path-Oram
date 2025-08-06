import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

plt.style.use('seaborn-v0_8')
sns.set_palette("husl")


chart_titles = {
    'main_titles': {
        'no_opt': 'No Optimization Performance Analysis',
        'forest_opt': 'Forest Optimization Performance Analysis', 
        'ring_oram': 'Ring ORAM Performance Analysis',
        'ring_oram_forest': 'Ring ORAM Forest Performance Analysis',
        'random_rr_forest': 'Random RR Forest Performance Analysis'
    },
    
    'subplot_titles': {
        'tree_200k': 'Tree Size: 200,000',
        'tree_1m': 'Tree Size: 1,000,000'
    },
    
    'axis_labels': {
        'x_label': 'Operation Size',
        'y_label_time': 'Average Time (ms)',
        'y_label_stash': 'Average Stash Size'
    },
    
    'legend_labels': {
        'time': 'Average Time',
        'stash': 'Average Stash'
    },
    
    'operation_labels': ['100k', '200k', '500k', '700k', '1M'],
    
    'stat_titles': {
        'summary_200k': 'Algorithm Summary (Tree Size: 200,000)',
        'summary_1m': 'Algorithm Summary (Tree Size: 1,000,000)',
        'detailed_table_200k': 'Detailed Performance Table (Tree Size: 200,000)',
        'detailed_table_1m': 'Detailed Performance Table (Tree Size: 1,000,000)',
        'comparison_200k': 'COMPREHENSIVE COMPARISON TABLE (Tree Size: 200,000)',
        'comparison_1m': 'COMPREHENSIVE COMPARISON TABLE (Tree Size: 1,000,000)',
        'rankings_200k': 'PERFORMANCE RANKINGS (Tree Size: 200,000)',
        'rankings_1m': 'PERFORMANCE RANKINGS (Tree Size: 1,000,000)',
        'time_ranking': 'Time per Operation Performance Ranking (Best to Worst):',
        'stash_ranking': 'Stash Size Ranking (Best to Worst):'
    },
    
    'table_headers': {
        'detailed_table': ['Operation Size', 'Tree Size', 'Avg Time/Op', 'Min Time/Op', 
                          'Max Time/Op', 'Avg Stash', 'Min Stash', 'Max Stash'],
        'comparison_table': ['Algorithm', 'Avg Time per Op (μs)', 'Min Time per Op (μs)', 
                            'Max Time per Op (μs)', 'Avg Stash', 'Min Stash', 'Max Stash',
                            'Time per Op Std Dev', 'Stash Std Dev']
    }
}

stash_ranges = {
    'no_opt_200k': (50, 60),
    'no_opt_1m': (60, 80),
    'forest_opt_200k': (50, 65),
    'forest_opt_1m': (720, 800),
    'ring_oram_200k': (49, 60),
    'ring_oram_1m': (58, 66),
    'ring_oram_forest_200k': (50, 61),
    'ring_oram_forest_1m': (725, 765),
    'random_rr_forest_200k': (100, 160),
    'random_rr_forest_1m': (1100, 1210)
}


colors = {
    'no_opt': '#1f77b4',
    'forest_opt': '#ff7f0e', 
    'ring_oram': '#2ca02c',
    'ring_oram_forest': '#d62728',
    'random_rr_forest': '#9467bd'
}

csv_files = {
    'no_opt': './csv/result_no_opt.csv',
    'forest_opt': './csv/result_forest_opt.csv',
    'ring_oram': './csv/result_ring_oram.csv',
    'ring_oram_forest': './csv/result_ring_oram_forest.csv',
    'random_rr_forest': './csv/result_random_rr_forest.csv'
}

dataframes = {}
for name, file in csv_files.items():
    dataframes[name] = pd.read_csv(file)

for name, df in dataframes.items():
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
    
    fig.suptitle(chart_titles['main_titles'][name], 
                 fontsize=16, fontweight='bold')
    
    subset_200k = df[df['tree_size'] == 200000]
    
    x_positions = [0, 1, 2, 3, 4]
    
    # Convert microseconds to milliseconds for plotting
    time_ms_200k = subset_200k['avg_time'] / 1000.0
    line1 = ax1.plot(x_positions, time_ms_200k, 
                     marker='o', linewidth=2, markersize=8, color='red', 
                     label=chart_titles['legend_labels']['time'])
    
    ax1_twin = ax1.twinx()
    line2 = ax1_twin.plot(x_positions, subset_200k['avg_stash'], 
                          marker='s', linewidth=2, markersize=8, color='blue', 
                          label=chart_titles['legend_labels']['stash'])
    
    for i, y in enumerate(time_ms_200k):
        ax1.annotate(f'{y:.3f}ms', (i, y), textcoords="offset points", 
                     xytext=(0,10), ha='center', fontsize=8)
    
    for i, y in enumerate(subset_200k['avg_stash']):
        ax1_twin.annotate(f'{y:.0f}', (i, y), textcoords="offset points", 
                          xytext=(0,-15), ha='center', fontsize=8)
    
    ax1.set_xlabel(chart_titles['axis_labels']['x_label'])
    ax1.set_ylabel(chart_titles['axis_labels']['y_label_time'], color='red')
    ax1_twin.set_ylabel(chart_titles['axis_labels']['y_label_stash'], color='blue')
    ax1_twin.set_ylim(stash_ranges[f'{name}_200k'])
    ax1.set_title(chart_titles['subplot_titles']['tree_200k'])
    ax1.grid(True, alpha=0.3)
    
    ax1.set_xticks(x_positions)
    ax1.set_xticklabels(chart_titles['operation_labels'])
    
    lines1, labels1 = ax1.get_legend_handles_labels()
    lines2, labels2 = ax1_twin.get_legend_handles_labels()
    ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper left')
    
    subset_1m = df[df['tree_size'] == 1000000]
    
    # Convert microseconds to milliseconds for plotting
    time_ms_1m = subset_1m['avg_time'] / 1000.0
    line3 = ax2.plot(x_positions, time_ms_1m, 
                     marker='o', linewidth=2, markersize=8, color='red', 
                     label=chart_titles['legend_labels']['time'])
    
    ax2_twin = ax2.twinx()
    line4 = ax2_twin.plot(x_positions, subset_1m['avg_stash'], 
                          marker='s', linewidth=2, markersize=8, color='blue', 
                          label=chart_titles['legend_labels']['stash'])
    
    for i, y in enumerate(time_ms_1m):
        ax2.annotate(f'{y:.3f}ms', (i, y), textcoords="offset points", 
                     xytext=(0,10), ha='center', fontsize=8)
    
    for i, y in enumerate(subset_1m['avg_stash']):
        ax2_twin.annotate(f'{y:.0f}', (i, y), textcoords="offset points", 
                          xytext=(0,-15), ha='center', fontsize=8)
    
    ax2.set_xlabel(chart_titles['axis_labels']['x_label'])
    ax2.set_ylabel(chart_titles['axis_labels']['y_label_time'], color='red')
    ax2_twin.set_ylabel(chart_titles['axis_labels']['y_label_stash'], color='blue')
    ax2_twin.set_ylim(stash_ranges[f'{name}_1m'])
    ax2.set_title(chart_titles['subplot_titles']['tree_1m'])
    ax2.grid(True, alpha=0.3)
    
    ax2.set_xticks(x_positions)
    ax2.set_xticklabels(chart_titles['operation_labels'])
    
    lines3, labels3 = ax2.get_legend_handles_labels()
    lines4, labels4 = ax2_twin.get_legend_handles_labels()
    ax2.legend(lines3 + lines4, labels3 + labels4, loc='upper left')
    
    plt.tight_layout()
    plt.show()
    

    subset_200k = df[df['tree_size'] == 200000].copy()

    # Convert microseconds to milliseconds for display
    subset_200k.loc[:, 'time_per_op'] = subset_200k['avg_time'] / subset_200k['operate_size'] / 1000.0
    
    print(f"\n=== {name.replace('_', ' ').title()} {chart_titles['stat_titles']['summary_200k']} ===")
    print(f"Average Time per Operation: {subset_200k['time_per_op'].mean() * 1000.0:.4f} μs")
    print(f"Average Stash Size: {subset_200k['avg_stash'].mean():.1f}")
    print(f"Time per Operation Range: {subset_200k['time_per_op'].min() * 1000.0:.4f} - {subset_200k['time_per_op'].max() * 1000.0:.4f} μs")
    print(f"Stash Range: {subset_200k['avg_stash'].min():.0f} - {subset_200k['avg_stash'].max():.0f}")
    
    print(f"\n{chart_titles['stat_titles']['detailed_table_200k']} for {name.replace('_', ' ').title()}:")
    print("=" * 80)
    print(f"{'Operation Size':<15} {'Tree Size':<12} {'Avg Time/Op':<12} {'Min Time/Op':<12} {'Max Time/Op':<12} {'Avg Stash':<12} {'Min Stash':<12} {'Max Stash':<12}")
    print("-" * 80)
    
    for _, row in subset_200k.iterrows():
        min_time_per_op = row['min_time'] / row['operate_size'] / 1000.0
        max_time_per_op = row['max_time'] / row['operate_size'] / 1000.0
        print(f"{row['operate_size']:<15} {row['tree_size']:<12} {row['time_per_op'] * 1000.0:<12.4f} {min_time_per_op * 1000.0:<12.4f} {max_time_per_op * 1000.0:<12.4f} {row['avg_stash']:<12.0f} {row['min_stash']:<12.0f} {row['max_stash']:<12.0f}")
    
    print("=" * 80)
    

    subset_1m = df[df['tree_size'] == 1000000].copy()
    # Convert microseconds to milliseconds for display
    subset_1m.loc[:, 'time_per_op'] = subset_1m['avg_time'] / subset_1m['operate_size'] / 1000.0
    
    print(f"\n=== {name.replace('_', ' ').title()} {chart_titles['stat_titles']['summary_1m']} ===")
    print(f"Average Time per Operation: {subset_1m['time_per_op'].mean() * 1000.0:.4f} μs")
    print(f"Average Stash Size: {subset_1m['avg_stash'].mean():.1f}")
    print(f"Time per Operation Range: {subset_1m['time_per_op'].min() * 1000.0:.4f} - {subset_1m['time_per_op'].max() * 1000.0:.4f} μs")
    print(f"Stash Range: {subset_1m['avg_stash'].min():.0f} - {subset_1m['avg_stash'].max():.0f}")
    
    print(f"\n{chart_titles['stat_titles']['detailed_table_1m']} for {name.replace('_', ' ').title()}:")
    print("=" * 80)
    print(f"{'Operation Size':<15} {'Tree Size':<12} {'Avg Time/Op':<12} {'Min Time/Op':<12} {'Max Time/Op':<12} {'Avg Stash':<12} {'Min Stash':<12} {'Max Stash':<12}")
    print("-" * 80)
    
    for _, row in subset_1m.iterrows():
        min_time_per_op = row['min_time'] / row['operate_size'] / 1000.0
        max_time_per_op = row['max_time'] / row['operate_size'] / 1000.0
        print(f"{row['operate_size']:<15} {row['tree_size']:<12} {row['time_per_op'] * 1000.0:<12.4f} {min_time_per_op * 1000.0:<12.4f} {max_time_per_op * 1000.0:<12.4f} {row['avg_stash']:<12.0f} {row['min_stash']:<12.0f} {row['max_stash']:<12.0f}")
    
    print("=" * 80)
    print()

print("\n" + "="*100)
print(chart_titles['stat_titles']['comparison_200k'])
print("="*100)

comparison_data_200k = []
for name, df in dataframes.items():
    subset_200k = df[df['tree_size'] == 200000].copy()
    # Convert microseconds to milliseconds for display
    subset_200k.loc[:, 'time_per_op'] = subset_200k['avg_time'] / subset_200k['operate_size'] / 1000.0
    comparison_data_200k.append({
        'Algorithm': name.replace('_', ' ').title(),
        'Avg Time per Op (μs)': subset_200k['time_per_op'].mean() * 1000.0,
        'Min Time per Op (μs)': subset_200k['time_per_op'].min() * 1000.0,
        'Max Time per Op (μs)': subset_200k['time_per_op'].max() * 1000.0,
        'Avg Stash': subset_200k['avg_stash'].mean(),
        'Min Stash': subset_200k['avg_stash'].min(),
        'Max Stash': subset_200k['avg_stash'].max(),
        'Time per Op Std Dev': subset_200k['time_per_op'].std() * 1000.0,
        'Stash Std Dev': subset_200k['avg_stash'].std()
    })

comparison_df_200k = pd.DataFrame(comparison_data_200k)
print(comparison_df_200k.to_string(index=False, float_format='%.4f'))

print("\n" + "="*100)
print(chart_titles['stat_titles']['comparison_1m'])
print("="*100)

comparison_data_1m = []
for name, df in dataframes.items():
    subset_1m = df[df['tree_size'] == 1000000].copy()
    # Convert microseconds to milliseconds for display
    subset_1m.loc[:, 'time_per_op'] = subset_1m['avg_time'] / subset_1m['operate_size'] / 1000.0
    comparison_data_1m.append({
        'Algorithm': name.replace('_', ' ').title(),
        'Avg Time per Op (μs)': subset_1m['time_per_op'].mean() * 1000.0,
        'Min Time per Op (μs)': subset_1m['time_per_op'].min() * 1000.0,
        'Max Time per Op (μs)': subset_1m['time_per_op'].max() * 1000.0,
        'Avg Stash': subset_1m['avg_stash'].mean(),
        'Min Stash': subset_1m['avg_stash'].min(),
        'Max Stash': subset_1m['avg_stash'].max(),
        'Time per Op Std Dev': subset_1m['time_per_op'].std() * 1000.0,
        'Stash Std Dev': subset_1m['avg_stash'].std()
    })

comparison_df_1m = pd.DataFrame(comparison_data_1m)
print(comparison_df_1m.to_string(index=False, float_format='%.4f'))

print("\n" + "="*80)
print(chart_titles['stat_titles']['rankings_200k'])
print("="*80)

time_ranking_200k = comparison_df_200k.sort_values('Avg Time per Op (μs)')
print(f"\n{chart_titles['stat_titles']['time_ranking']}")
for i, (_, row) in enumerate(time_ranking_200k.iterrows(), 1):
    print(f"{i}. {row['Algorithm']}: {row['Avg Time per Op (μs)']:.4f} μs")

stash_ranking_200k = comparison_df_200k.sort_values('Avg Stash')
print(f"\n{chart_titles['stat_titles']['stash_ranking']}")
for i, (_, row) in enumerate(stash_ranking_200k.iterrows(), 1):
    print(f"{i}. {row['Algorithm']}: {row['Avg Stash']:.1f}")

print("\n" + "="*80)
print(chart_titles['stat_titles']['rankings_1m'])
print("="*80)

time_ranking_1m = comparison_df_1m.sort_values('Avg Time per Op (μs)')
print(f"\n{chart_titles['stat_titles']['time_ranking']}")
for i, (_, row) in enumerate(time_ranking_1m.iterrows(), 1):
    print(f"{i}. {row['Algorithm']}: {row['Avg Time per Op (μs)']:.4f} μs")

stash_ranking_1m = comparison_df_1m.sort_values('Avg Stash')
print(f"\n{chart_titles['stat_titles']['stash_ranking']}")
for i, (_, row) in enumerate(stash_ranking_1m.iterrows(), 1):
    print(f"{i}. {row['Algorithm']}: {row['Avg Stash']:.1f}")

print("\n" + "="*80) 
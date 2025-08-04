#!/usr/bin/env python3
"""
VOC Data Visualizer for WHY2025 Badge
Visualizes collected VOC data to help identify patterns for ML training
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import argparse
import numpy as np
from datetime import datetime

def load_data(filename):
    """Load VOC data from CSV file"""
    try:
        df = pd.read_csv(filename)
        df['timestamp'] = pd.to_datetime(df['timestamp'])
        print(f"Loaded {len(df)} samples from {filename}")
        return df
    except Exception as e:
        print(f"Error loading data: {e}")
        return None

def plot_time_series(df):
    """Plot VOC values over time"""
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 10), sharex=True)
    
    # Group by label for coloring
    for label in df['label'].unique():
        label_data = df[df['label'] == label]
        ax1.scatter(label_data['timestamp'], label_data['voc'], label=label, alpha=0.6)
        ax2.scatter(label_data['timestamp'], label_data['temperature'], label=label, alpha=0.6)
        ax3.scatter(label_data['timestamp'], label_data['humidity'], label=label, alpha=0.6)
    
    ax1.set_ylabel('VOC Value')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    ax1.set_title('VOC Sensor Readings Over Time')
    
    ax2.set_ylabel('Temperature (°C)')
    ax2.grid(True, alpha=0.3)
    
    ax3.set_ylabel('Humidity (%)')
    ax3.set_xlabel('Time')
    ax3.grid(True, alpha=0.3)
    
    plt.tight_layout()
    return fig

def plot_distributions(df):
    """Plot VOC distributions by label"""
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # VOC distribution by label
    ax1 = axes[0, 0]
    for label in df['label'].unique():
        label_data = df[df['label'] == label]['voc']
        ax1.hist(label_data, bins=30, alpha=0.5, label=label, density=True)
    ax1.set_xlabel('VOC Value')
    ax1.set_ylabel('Density')
    ax1.set_title('VOC Distribution by Label')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Box plot
    ax2 = axes[0, 1]
    df.boxplot(column='voc', by='label', ax=ax2)
    ax2.set_title('VOC Values by Label')
    ax2.set_xlabel('Label')
    ax2.set_ylabel('VOC Value')
    
    # Scatter plot: VOC vs Temperature
    ax3 = axes[1, 0]
    for label in df['label'].unique():
        label_data = df[df['label'] == label]
        ax3.scatter(label_data['temperature'], label_data['voc'], label=label, alpha=0.5)
    ax3.set_xlabel('Temperature (°C)')
    ax3.set_ylabel('VOC Value')
    ax3.set_title('VOC vs Temperature')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # Scatter plot: VOC vs Humidity
    ax4 = axes[1, 1]
    for label in df['label'].unique():
        label_data = df[df['label'] == label]
        ax4.scatter(label_data['humidity'], label_data['voc'], label=label, alpha=0.5)
    ax4.set_xlabel('Humidity (%)')
    ax4.set_ylabel('VOC Value')
    ax4.set_title('VOC vs Humidity')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    return fig

def print_statistics(df):
    """Print statistical summary of the data"""
    print("\n=== Data Statistics ===")
    print(f"Total samples: {len(df)}")
    print(f"\nSamples by label:")
    print(df['label'].value_counts())
    
    print("\n=== VOC Statistics by Label ===")
    stats = df.groupby('label')['voc'].agg(['mean', 'std', 'min', 'max', 'count'])
    print(stats)
    
    print("\n=== Environmental Conditions ===")
    print(f"Temperature range: {df['temperature'].min():.1f}°C - {df['temperature'].max():.1f}°C")
    print(f"Humidity range: {df['humidity'].min():.1f}% - {df['humidity'].max():.1f}%")
    
    # Check for overlap in VOC ranges
    print("\n=== VOC Range Analysis ===")
    for label in df['label'].unique():
        voc_values = df[df['label'] == label]['voc']
        print(f"{label}: {voc_values.min():.0f} - {voc_values.max():.0f} (mean: {voc_values.mean():.0f})")

def plot_correlation_heatmap(df):
    """Plot correlation heatmap"""
    fig, ax = plt.subplots(figsize=(8, 6))
    
    # Calculate correlation matrix
    numeric_cols = ['voc', 'temperature', 'humidity']
    corr_matrix = df[numeric_cols].corr()
    
    # Plot heatmap
    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, 
                square=True, linewidths=1, ax=ax)
    ax.set_title('Feature Correlation Heatmap')
    
    return fig

def analyze_separability(df):
    """Analyze how well different smoke types can be separated"""
    print("\n=== Separability Analysis ===")
    
    labels = df['label'].unique()
    if len(labels) < 2:
        print("Need at least 2 different labels for separability analysis")
        return
    
    # Calculate pairwise separability
    from scipy import stats
    
    for i in range(len(labels)):
        for j in range(i+1, len(labels)):
            label1, label2 = labels[i], labels[j]
            voc1 = df[df['label'] == label1]['voc']
            voc2 = df[df['label'] == label2]['voc']
            
            # T-test
            t_stat, p_value = stats.ttest_ind(voc1, voc2)
            
            # Cohen's d (effect size)
            pooled_std = np.sqrt((voc1.std()**2 + voc2.std()**2) / 2)
            cohens_d = abs(voc1.mean() - voc2.mean()) / pooled_std
            
            print(f"\n{label1} vs {label2}:")
            print(f"  Mean difference: {abs(voc1.mean() - voc2.mean()):.1f}")
            print(f"  T-test p-value: {p_value:.4f}")
            print(f"  Cohen's d: {cohens_d:.2f} ({'small' if cohens_d < 0.5 else 'medium' if cohens_d < 0.8 else 'large'} effect)")

def main():
    parser = argparse.ArgumentParser(description='VOC Data Visualizer for WHY2025 Badge')
    parser.add_argument('file', help='CSV file with VOC data')
    parser.add_argument('--save', action='store_true', help='Save plots to files')
    parser.add_argument('--no-show', action='store_true', help='Do not display plots')
    
    args = parser.parse_args()
    
    # Load data
    df = load_data(args.file)
    if df is None:
        return
    
    # Print statistics
    print_statistics(df)
    analyze_separability(df)
    
    # Create plots
    fig1 = plot_time_series(df)
    fig2 = plot_distributions(df)
    fig3 = plot_correlation_heatmap(df)
    
    # Save plots if requested
    if args.save:
        base_name = args.file.replace('.csv', '')
        fig1.savefig(f'{base_name}_timeseries.png', dpi=150, bbox_inches='tight')
        fig2.savefig(f'{base_name}_distributions.png', dpi=150, bbox_inches='tight')
        fig3.savefig(f'{base_name}_correlation.png', dpi=150, bbox_inches='tight')
        print(f"\nPlots saved with prefix: {base_name}")
    
    # Show plots
    if not args.no_show:
        plt.show()

if __name__ == '__main__':
    main()
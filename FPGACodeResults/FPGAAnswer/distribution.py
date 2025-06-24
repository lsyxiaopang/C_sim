import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.stats import gaussian_kde

# 设置 Seaborn 样式
sns.set(style='whitegrid')

# 替换成你存放 CSV 文件的文件夹路径
folder_path = 'F:\Githublibs\C_sim\FPGACodeResults\FPGAAnswer'

# 获取所有 CSV 文件名
csv_files = [f for f in os.listdir(folder_path) if (f.endswith('.csv') and not f.startswith('prob'))]#*不要重复了

# 创建绘图
plt.figure(figsize=(10, 6))

out_kde_df=pd.DataFrame()
x_value=np.linspace(0,12,1000)
out_kde_df["x_val"]=x_value

# 遍历每个 CSV 文件
for file in csv_files:
    file_path = os.path.join(folder_path, file)
    df = pd.read_csv(file_path)

    # 提取第二列 TTS
    its = df.iloc[:, 1]
    its=its.fillna(np.inf)

    # 去除非正值（log会报错）
    its = its[its > 0]
    tts=its

    # 归一化
    # tts_normalized = (tts - tts.min()) / (tts.max() - tts.min() + 1e-9)

    # 绘制 KDE 曲线（横坐标用 log10）
    log_tts = np.log10(tts + 1e-6)  # 防止 log(0)
    sns.kdeplot(log_tts, label=file)
    kde=gaussian_kde(log_tts)
    y_value=kde.evaluate(x_value)
    out_kde_df[file]=y_value

# 添加图例和标签
plt.xlabel('log10(Normalized ITS)')
plt.ylabel('Density')
plt.title('KDE of Normalized ITS (log scale)')
plt.legend()
plt.tight_layout()
plt.savefig('KDE of Normalized ITS (log scale).png')
plt.show()
out_kde_df.to_csv("prob_kde_data.csv",index=False)

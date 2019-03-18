import csv
import struct
import os
import numpy as np
import pandas as pd
from PIL import Image
from sklearn import metrics
from sklearn.externals import joblib
from sklearn.ensemble import RandomForestClassifier

def trainModel(model, modelDir):
	# 读取训练数据集
	print("Loading training set.")
	# 读取图像
	with open("./MNIST/train-images.idx3-ubyte", 'rb') as imagesFile:
		magicNumber, numberOfImages, rows, cols = struct.unpack('>IIII', imagesFile.read(16))
		trainImages = np.fromfile(imagesFile, dtype=np.uint8).reshape(numberOfImages, 28 * 28)
		trainImages = trainImages.astype('int32')
	# 读取标签
	with open("./MNIST/train-labels.idx1-ubyte", 'rb') as labelsFile:
		magicNumber, numberOfItems = struct.unpack('>II', labelsFile.read(8))
		trainLabels = np.fromfile(labelsFile, dtype=np.uint8)
		trainLabels = trainLabels.astype('int32')

	# 开始训练
	print("Start training.")
	model.fit(trainImages, trainLabels)
	print("Finish training.")

	# 保存模型
	print("Saving model.")
	joblib.dump(model, modelDir)
	model = joblib.load(modelDir)
	
	# 读取测试数据集
	print("Loading testing set.")
	# 读取图像
	with open("./MNIST/t10k-images.idx3-ubyte", 'rb') as imagesFile:
		magicNumber, numberOfImages, rows, cols = struct.unpack('>IIII', imagesFile.read(16))
		testImages = np.fromfile(imagesFile, dtype=np.uint8).reshape(numberOfImages, 28 * 28)
		testImages = testImages.astype('int32')
	# 读取标签
	with open("./MNIST/t10k-labels.idx1-ubyte", 'rb') as labelsFile:
		magicNumber, numberOfItems = struct.unpack('>II', labelsFile.read(8))
		testLabels = np.fromfile(labelsFile, dtype=np.uint8)
		testLabels = testLabels.astype('int32')

	# 预测测试数据集，计算准确率
	predictedLabels = list(model.predict(testImages))
	print("Accuracy:", metrics.accuracy_score(testLabels, predictedLabels))

# 初始化随机森林模型
model = RandomForestClassifier(n_estimators=256, n_jobs=-1)

modelDir = './MNIST/classifier'
if(os.path.isfile(modelDir)):
	# 加载模型
	model = joblib.load(modelDir)
else:
	# 训练模型
	trainModel(model, modelDir)


imageDir = [] # 手写数字图像的路径
corner = []   # A4纸的角点

# 输出到excel的列
filename = []
corner1 = []
corner2 = []
corner3 = []
corner4 = []
studentID = []
phone = []
ID = []

# 获取图像路径
print('Reading images directory.')
f = open('./imageDir.txt')
imagedir = f.readline()
while imagedir:
	imagedir = imagedir.strip('\n')
	imageDir.append(imagedir)
	imagedir = f.readline()
f.close()

# 获取A4纸角点信息
f = open('./corner.txt')
c = f.readline()
while c:
	corner.append(c)
	c = f.readline()
for i in corner:
	info = i.split()
	filename.append(info[0])
	corner1.append(info[1] + ' ' + info[2])
	corner2.append(info[3] + ' ' + info[4])
	corner3.append(info[5] + ' ' + info[6])
	corner4.append(info[7] + ' ' + info[8])

# 从各个文件夹中读取图像
for dir in imageDir:
	images = []
	print('Reading images from directory: %s' % dir)
	f = open(dir + '/imageList.txt')
	row = f.readline()
	while row:
		row = row.strip('\n')
		row = row.split()
		images.append(row)
		row = f.readline()
	f.close()
	# 预测三行
	for x in range(0, 3):
		results = ""
		# 读取单个数字并预测
		for i in images[x]:
			imageName = dir + '/' + i
			img = Image.open(imageName)
			img = img.resize((28, 28))
			arr = np.array(img)
			arr = arr[:,:]
			arr = arr.reshape(1, 28 * 28)
			res = int(model.predict(arr.copy()))
			results = results + str(res)
		if x == 0: # 预测学号
			print("Predicted student id:")
			studentID.append(results)
		elif x == 1: # 预测手机号
			print("Predicted phone number:")
			phone.append(results)
		else: # 预测身份证号
			print("Predicted ID:")
			ID.append(results)
		print(results)

writer = pd.ExcelWriter('Result/Step4/predict.xlsx', engine='xlsxwriter')
df = pd.DataFrame({'文件名': filename, '角点1': corner1, '角点2': corner2, 
					'角点3': corner3, '角点4': corner4, '学号': studentID, 
					'手机号': phone, '身份证号': ID})
df.to_excel(writer, sheet_name='Sheet1', index=False)
writer.save()
writer.close()
print("Saved predicted results to 'Result/Step4/predict.xlsx'.")
input("Press any key to exit.")
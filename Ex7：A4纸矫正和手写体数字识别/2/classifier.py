import csv
import struct
import os
import numpy as np
from PIL import Image
from sklearn import metrics
from sklearn.ensemble import AdaBoostClassifier
from sklearn.externals import joblib
from sklearn import tree

def loadDataSet(path, category = 'train'):
	# 获取文件路径
	labelsPath = os.path.join(path, '%s-labels.idx1-ubyte' % category)
	imagesPath = os.path.join(path, '%s-images.idx3-ubyte' % category)
	# 读取标签
	with open(labelsPath, 'rb') as labelsFile:
		magicNumber, numberOfItems = struct.unpack('>II', labelsFile.read(8))
		labels = np.fromfile(labelsFile, dtype=np.uint8)
	# 读取图像
	with open(imagesPath, 'rb') as imagesFile:
		magicNumber, numberOfImages, rows, cols = struct.unpack('>IIII', imagesFile.read(16))
		images = np.fromfile(imagesFile, dtype=np.uint8).reshape(len(labels), 28 * 28)
	return images, labels

def trainModel(abc, modelDir):
	# 读取训练数据集
	print("Loading training set.")
	# 读取图像
	with open("./train-images.idx3-ubyte", 'rb') as imagesFile:
		magicNumber, numberOfImages, rows, cols = struct.unpack('>IIII', imagesFile.read(16))
		trainImages = np.fromfile(imagesFile, dtype=np.uint8).reshape(numberOfImages, 28 * 28)
		trainImages = trainImages.astype('int32')
	# 读取标签
	with open("./train-labels.idx1-ubyte", 'rb') as labelsFile:
		magicNumber, numberOfItems = struct.unpack('>II', labelsFile.read(8))
		trainLabels = np.fromfile(labelsFile, dtype=np.uint8)
		trainLabels = trainLabels.astype('int32')

	# 开始训练
	print("Start training.")
	model = abc.fit(trainImages, trainLabels)
	print("Finish training.")

	# 保存模型
	print("Saving model.")
	joblib.dump(abc, modelDir)
	abc = joblib.load(modelDir)
	
	# 读取测试数据集
	print("Loading testing set.")
	# 读取图像
	with open("./t10k-images.idx3-ubyte", 'rb') as imagesFile:
		magicNumber, numberOfImages, rows, cols = struct.unpack('>IIII', imagesFile.read(16))
		testImages = np.fromfile(imagesFile, dtype=np.uint8).reshape(numberOfImages, 28 * 28)
		testImages = testImages.astype('int32')
	# 读取标签
	with open("./t10k-labels.idx1-ubyte", 'rb') as labelsFile:
		magicNumber, numberOfItems = struct.unpack('>II', labelsFile.read(8))
		testLabels = np.fromfile(labelsFile, dtype=np.uint8)
		testLabels = testLabels.astype('int32')

	# 预测测试数据集，计算准确率
	predictedLabels = list(model.predict(testImages))
	print("Accuracy:", metrics.accuracy_score(testLabels, predictedLabels))

# 初始化决策树分类器，作为AdaBoost的弱分类器
dtc = tree.DecisionTreeClassifier()
# 初始化AdaBoost分类器
abc = AdaBoostClassifier(base_estimator = dtc)

modelDir = './classifier'
if(os.path.isfile(modelDir)):
	# 加载模型
	abc = joblib.load(modelDir)
else:
	# 训练模型
	trainModel(abc, modelDir)

# 预测自定义数据集
i = 0
while i < 10:
	# 读取图片
	path = str(i) + ".jpg"
	image = Image.open(path)
	image = image.resize((28, 28))
	# 获取图片数据的数组
	imageArray = np.array(image)
	imageArray = imageArray[:,:,:1]
	imageArray = imageArray.reshape(1, 28 * 28)
	# 预测
	print(int(abc.predict(imageArray.copy())))
	i = i + 1

input("Press any key to exit.")
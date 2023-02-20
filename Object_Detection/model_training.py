# -*- coding: utf-8 -*-
"""
Train and save models 
- NN
- SVM
- KNN
- GNB 

@author: Gwangwon Kim
version 2.4
"""
import os
import glob
import numpy as np
import pandas as pd
import pickle
import module
import joblib
import time

import tensorflow as tf
from sklearn import svm
from tensorflow.keras.optimizers import Adam
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
from keras.utils import to_categorical
from tensorflow.keras.models import Sequential, Model
from tensorflow.keras.layers import Input, Dense , Conv2D , Dropout , Flatten , Activation, MaxPooling2D , GlobalAveragePooling2D
from tensorflow.keras.optimizers import Adam , RMSprop 
from tensorflow.keras.layers import BatchNormalization
from tensorflow.keras.callbacks import ReduceLROnPlateau , EarlyStopping , ModelCheckpoint , LearningRateScheduler
from skl2onnx import convert_sklearn # (v1.13)
from skl2onnx.common.data_types import FloatTensorType

# Setting(For using GPU)
physical_devices = tf.config.list_physical_devices('GPU')
try:
    tf.config.experimental.set_memory_growth(physical_devices[0],True)
except:
    pass

# Data input - pkl format
df = pd.read_pickle("save/tonnetz_3.pkl")
# processing
X = np.array(df.feature.tolist())
y = np.array(df.class_label.tolist())
x_train, x_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=0)
## for Neural network
y_oh = to_categorical(y) # make 2-dimensions
x_train, x_test, y_train_nn, y_test_nn = train_test_split(X, y_oh, test_size=0.2, random_state=0)
time_dict = {} # Measure training time

# NN
BATCH_SIZE = 32
EPOCHS = 15
## training
nn_model_start_time = time.time()
nn_model = module.neural_base(x_train.shape[1]) # Match with column
nn_model.compile(optimizer=Adam(), loss='categorical_crossentropy', metrics=['accuracy'])
nn_history = nn_model.fit(x_train, y_train_nn, batch_size=BATCH_SIZE, epochs=EPOCHS, validation_split = 0.1)
#nn_model.save('save/nn_model.h5') # Save model and weights(.pb)
time_dict['NN'] = time.time() - nn_model_start_time
## Model evaluate
#nn_accuracy = nn_model.evaluate(x_test_nn, y_test_nn)[1]
nn_model.predict(x_test)
predicted_class = np.argmax(nn_model.predict(x_test), axis=1)
print("----- NN model -----\n", classification_report(y_test, predicted_class))                       

# SVM
## training
svm_start_time = time.time()
svm_model = module.svm_base(C=10)
svm_model.fit(x_train, y_train)
joblib.dump(svm_model, 'save/svm_model.pkl') # save model
time_dict['SVM'] = time.time() - svm_start_time

## Model evaluate
#svm_accuracy = svm_model.score(x_test, y_test)
print("----- SVM model -----\n", classification_report(y_test, svm_model.predict(x_test)))

# KNN
## training
knn_start_time = time.time()
knn_model = module.knn_base(n_neighbors=6)
knn_model.fit(x_train, y_train)
#joblib.dump(knn_model, 'save/knn_model.pkl') # save model
time_dict['KNN'] = time.time() - knn_start_time

## Model evaluate
#knn_accuracy = knn_model.score(x_test, y_test)
print("----- KNN model -----\n", classification_report(y_test, knn_model.predict(x_test)))

# GNB
## training
gnb_start_time = time.time()
gnb_model = module.gnb_base()
gnb_model.fit(x_train, y_train)
#joblib.dump(gnb_model, 'save/gnb_model.pkl') # save model
time_dict['GNB'] = time.time() - gnb_start_time

## Model evaluate
#gnb_accuracy = gnb_model.score(x_test, y_test)
print("----- GNB model -----\n", classification_report(y_test, gnb_model.predict(x_test)))

# inference time
#print(time_dict)


'''
# CNN - appendix
x_train_cnn = tf.reshape(x_train,[160,40,1])
x_test_cnn = tf.reshape(x_test, [40,40,1])
cnn_model = module.cnn_base(40,1)
cnn_model.compile(optimizer=Adam(), loss='binary_crossentropy', metrics=['accuracy'])
cnn_history = cnn_model.fit(x_train_cnn, y_train_oh, batch_size=BATCH_SIZE, epochs=EPOCHS, validation_split = 0.1)
# Model evaluate
cnn_accuracy = cnn_model.evaluate(x_test_cnn, y_test_oh)[1]'''






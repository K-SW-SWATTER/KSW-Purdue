# -*- coding: utf-8 -*-
"""
Version 1.2
@author: Gwangwon Kim
"""
import os
import numpy as np
import pandas as pd
from sklearn import svm, naive_bayes, neighbors
import matplotlib.pyplot as plt
from tensorflow.keras.models import Model
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.layers import Input, Dense, Dropout, Conv1D, Conv2D, Flatten, Activation, MaxPooling2D , GlobalAveragePooling2D
import joblib
import librosa
from tensorflow.keras.optimizers import Adam , RMSprop 
from tensorflow.keras.layers import BatchNormalization
from tensorflow.keras.callbacks import ReduceLROnPlateau , EarlyStopping , ModelCheckpoint , LearningRateScheduler
import warnings
warnings.filterwarnings(action='ignore')


# Feature Extraction
def extract_feature(signal, sr, feature):
    # select feature
    if feature == 'mfcc':
        data = np.mean(librosa.feature.mfcc(signal,sr=sr,n_mfcc=40).T, axis=0)
    elif feature == 'mel':
        data = np.mean(librosa.feature.melspectrogram(signal,sr=sr,n_mels=40).T,axis=0)
    elif feature == 'chroma':
        data = np.mean(librosa.feature.chroma_stft(signal, sr).T, axis=0)
    elif feature == 'contrast':
           data = np.mean(librosa.feature.spectral_contrast(S=np.abs(librosa.stft(signal)),sr=sr).T,axis=0)
    elif feature == 'tonnetz':
        data = np.mean(librosa.feature.tonnetz(y=librosa.effects.harmonic(signal),sr=sr).T,axis=0)
    else:
        print("Error when extract feature")
        return None
    return data


# Convert data to dataframe
def convert_data(length,company,label, feature):
    features = []     
    for index in range(1,length+1):
        file_name = 'Dataset/'+ company + '/' + str(index) + '.wav'
        # mfcc extract
        signal, sr = librosa.load(file_name, sr=22050)
        data = extract_feature(signal, sr, feature)
        features.append([data,label]) # drone class
        featuredf = pd.DataFrame(features, columns=['feature', 'class_label'])
    return featuredf


# Concat and Save dataframe
## feature : mfcc, mel, chroma, contrast, tonnetz
def concat_data(feature):
    autel_df = convert_data(100, 'Autel_Evo2', 0, feature)
    dji_df = convert_data(100, 'DJI_Phantom4', 1, feature)
    noise_df = convert_data(200, 'noise', 2, feature)
    df = pd.concat([autel_df, dji_df, noise_df])
    df.to_pickle('save/' + feature +  '_3.pkl') # save dataframe to pickle
    print(feature + ' save complete')

# Modeling
## SVM(Support Vector Machine)
def svm_base(C):
    svm_model = svm.LinearSVC(C=C, loss='squared_hinge')
    return svm_model
## libSVM for implementation

## GNB (Gaussian Naive Bayes)
def gnb_base():
    gnb_model = naive_bayes.GaussianNB()
    return gnb_model

## KNN (K-Nearest-Neighbor)
def knn_base(n_neighbors=6):
    knn_model = neighbors.KNeighborsClassifier(n_neighbors=n_neighbors)
    return knn_model

## NN(Nueral Network)
def neural_base(column):
    input_tensor = Input(shape=(column))
    x = Dense(128)(input_tensor)
    x = Activation('relu')(x)
    x = Dropout(rate=0.1)(x)
    x = Dense(128)(x)
    x = Activation('relu')(x)
    x = Dropout(rate=0.1)(x)

    output = Dense(3, activation='sigmoid')(x)
    
    model = Model(inputs=input_tensor, outputs=output)
    model.summary()
    return model

## CNN(Convolutional Nueral Network) - appendix
def cnn_base(column, channel):
    input_tensor = Input(shape=(column, channel))
    
    x = Conv1D(16,16, activation='relu')(input_tensor)
    x = Conv1D(16,16, activation='relu')(x)
    
    x = Flatten()(x)
    
    x = Dense(32, activation = 'relu')(x)
    x = Dropout(rate=0.2)(x)
    
    output = Dense(3, activation='softmax', name='output')(x)
    
    model = Model(inputs=input_tensor, outputs=output)
    model.summary()
    return model

# Visualize training graph
def show_history(history):
    plt.figure(figsize=(6,6))
    plt.yticks(np.arange(0,1,0.05))
    plt.plot(history.history['accuracy'], label='train')
    plt.plot(history.history['val_accuracy'], label='valid')
    plt.legend()

if __name__ == '__main__':
    # Make dataframe using UAV data
    concat_data('tonnetz')
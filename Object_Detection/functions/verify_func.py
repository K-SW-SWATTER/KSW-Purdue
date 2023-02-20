# -*- coding: utf-8 -*-
"""
* Verify SVM conversion into tf model.
* Save tf models.

@author: Gwangwon Kim

version 1.1
"""
import pandas as pd
import numpy as np
import tensorflow as tf
import time
from sklearn.model_selection import train_test_split
from sklearn import svm
import librosa
from tensorflow.keras.optimizers.experimental import Adadelta


# Verify svm conversion into tensorflow
# Data input - pkl format
def verify_svm():
    df = pd.read_pickle("../save/mfcc_3.pkl")
    # processing
    X = np.array(df.feature.tolist())
    y = np.array(df.class_label.tolist())
    x_train, x_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=20)
    
    # SVM
    ## training
    svm_model = svm.LinearSVC(C=10, loss='squared_hinge')
    svm_model.fit(x_train,y_train)
    
    # Creat a simple tfmodel for same architecture
    tf_model = tf.keras.models.Sequential()
    tf_model.add(tf.keras.Input(shape=(40,)))
    tf_model.add(tf.keras.layers.Dense(3))
    
    tf_model.layers[0].weights[0].assign(svm_model.coef_.transpose())
    tf_model.layers[0].bias.assign(svm_model.intercept_)
    
    # Verify model's results
    print((svm_model.predict(x_train) == np.argmax(tf_model(x_train),axis=1)))
    
    # Compile tf model
    tf_model.compile(loss='squared_hinge', optimizer=Adadelta(), metrics=['accuracy'])
    tf_model.fit(x_train, y_train)
    # Save model file
    tf_model.save('../save/tf_svm_model.h5')


# Verify shape of features
def verify_feature():
    df = pd.DataFrame(index = ['MFCC','mel','chroma_stft','contrast','tonnetz']
                      ,columns = ['Shape'])
    ## sample data
    y, sr = librosa.load(librosa.ex('trumpet'), sr=22050)
    
    ## MFCC
    mfcc = librosa.feature.mfcc(y, sr=sr, n_mfcc=40)
    mfcc_mean = np.mean(mfcc.T,axis=0)
    print('mfcc: ', mfcc.shape, 'mfcc_mean:', mfcc_mean.shape)
    df.iloc[0] = mfcc.shape[0]
    
    ## mel
    mel = librosa.feature.melspectrogram(y,sr=sr)
    mel_mean = np.mean(mel.T, axis=0)
    print('mel: ', mel.shape, 'mel_mean:', mel_mean.shape)
    df.iloc[1] = mel.shape[0]
    
    ## chroma_stft
    chroma_stft = librosa.feature.chroma_stft(y,sr)
    chroma_stft_mean = np.mean(chroma_stft.T, axis=0)
    print('stft: ', chroma_stft.shape, 'stft_mean:', chroma_stft_mean.shape)
    df.iloc[2] = chroma_stft.shape[0]
    
    ## contrast
    stft = np.abs(librosa.stft(y))
    contrast = librosa.feature.spectral_contrast(S=stft,sr=sr)
    contrast_mean = np.mean(contrast.T, axis=0)
    print('contrast: ', contrast.shape, 'contrast_mean:', contrast_mean.shape)
    df.iloc[3] = contrast.shape[0]
    
    ## tonnetz
    tonnetz = librosa.feature.tonnetz(y=librosa.effects.harmonic(y),sr=sr)
    tonnetz_mean = np.mean(tonnetz.T, axis=0)
    print('tonntez: ', tonnetz.shape, 'tonnetz_mean:', tonnetz_mean.shape)
    df.iloc[4] = tonnetz.shape[0]
        
    print(df)
        
if __name__ == '__main__':
    verify_svm()
    verify_feature()



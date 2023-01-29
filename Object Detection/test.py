# -*- coding: utf-8 -*-
"""
Test File

@author: Gwangwon Kim


"""
import librosa
import numpy as np

# function test
def test():
    print('test')
    
def test2(word):
    word()
    
test2(test)

# feature extraction test
y, sr = librosa.load(librosa.ex('trumpet'))
print(y)

## MFCC
mfcc = librosa.feature.mfcc(y, sr=sr)
mfcc = np.mean(mfcc.T,axis=0)
print(mfcc.shape)

## mel
mel = librosa.feature.melspectrogram(y,sr=sr)
mel = np.mean(mel.T, axis=0)
print(mel.shape)

## stft
stft = np.abs(librosa.stft(y))
print(stft.shape)

## contrast
contrast = librosa.feature.spectral_contrast(S=stft,sr=sr)
contrast = np.mean(contrast.T, axis=0)
print(contrast.shape)

## tonnetz
tonnetz = librosa.feature.tonnetz(y=librosa.effects.harmonic(y),sr=sr)
tonnetz = np.mean(tonnetz.T, axis=0)
print(tonnetz.shape)
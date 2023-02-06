# -*- coding: utf-8 -*-
"""
Test File

@author: Gwangwon Kim


"""
import librosa
import numpy as np
import pandas as pd

# function test
def test():
    print('test')
    
def test2(word):
    word()
    
test2(test)

# feature extraction test
## Verify shape of features
df = pd.DataFrame(index = ['MFCC','mel','chroma_stft','contrast','tonnetz']
                  ,columns = ['Shape'])
#df.index.name = 'Features'

## sample data
y, sr = librosa.load(librosa.ex('trumpet'))
print(y.shape)
print(y)

## MFCC
mfcc = librosa.feature.mfcc(y, sr=sr, n_mfcc=40)
mfcc = np.mean(mfcc.T,axis=0)
# print(mfcc)
df.iloc[0] = mfcc.shape[0]

## mel
mel = librosa.feature.melspectrogram(y,sr=sr)
mel = np.mean(mel.T, axis=0)
df.iloc[1] = mel.shape[0]

## chroma_stft
chroma_stft = librosa.feature.chroma_stft(y,sr)
chroma_stft = np.mean(chroma_stft.T, axis=0)
df.iloc[2] = chroma_stft.shape[0]

## contrast
stft = np.abs(librosa.stft(y))
contrast = librosa.feature.spectral_contrast(S=stft,sr=sr)
contrast = np.mean(contrast.T, axis=0)
df.iloc[3] = contrast.shape[0]

## tonnetz
tonnetz = librosa.feature.tonnetz(y=librosa.effects.harmonic(y),sr=sr)
tonnetz = np.mean(tonnetz.T, axis=0)
df.iloc[4] = tonnetz.shape[0]

print(df)
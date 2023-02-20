# -*- coding: utf-8 -*-
"""
Data processing functions

@author: Gwangwon Kim
Version 2.0
"""
import tensorflow as tf
from tensorflow import keras
import os
from pydub import AudioSegment
import math
import librosa
from librosa import display
import matplotlib.pyplot as plt
from sklearn.preprocessing import scale

# Export models to other format.
# (.h5) -> (.pb) -> (.tflite)
def convert_func():
    # Convert .h5 model to .pb
    model = keras.models.load_model('../save/tf_svm_model.h5', compile=False)

    export_path = '../save/pb'
    model.save(export_path, save_format='tf')

    # Convert Tensorflow Lite model for app
    saved_model_dir = '../save/pb'
    converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir)
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS,
                                       tf.lite.OpsSet.SELECT_TF_OPS]
    tflite_model = converter.convert()
    open('../save/tf_svm_model.tflite', 'wb').write(tflite_model)

    
# Change name of audio file
def name_change():
    file_path = '../Dataset/noise'
    file_names = os.listdir(file_path)
    i = 1
    for name in file_names:
        src = os.path.join(file_path, name)
        dst = str(i)+'.wav'
        dst = os.path.join(file_path, dst)
        os.rename(src,dst)
        i+=1
 
# Trim audio files for 5 seconds
def trim_func():
    # Audio file path
    audio = AudioSegment.from_wav("test-1.wav")
    seconds = 5 * 1000 # set 5 seconds

    for i in range(int(math.floor(len(audio)/seconds))):
        slice = audio[i*seconds:seconds*(i+1)]
        slice.export('../Dataset/trim_test/{}.wav'.format(i), format='wav')


# Visualize audio data's feature
def visualize_feature(feature):
    uav_path = '../Dataset/Autel_Evo2/5.wav'
    noise_path = '../Dataset/noise/5.wav'
    y_uav, sr_uav = librosa.load(uav_path, sr=22050)
    y_noise, sr_noise = librosa.load(noise_path, sr=22050)
    
    # MFCC
    if feature == 'mfcc':
        mfcc_uav = librosa.feature.mfcc(y_uav, sr_uav, n_mfcc=40)
        mfcc_uav_scale = scale(mfcc_uav, axis=1)
        mfcc_noise = librosa.feature.mfcc(y_noise,sr_noise,n_mfcc=40)
        mfcc_noise_scale = scale(mfcc_noise, axis=1)
        
        plt.figure(figsize=(14, 9))
        ax1 = plt.subplot(2,1,1)
        librosa.display.specshow(mfcc_uav_scale, x_axis='time')
        plt.title('UAV')
        ax2 = plt.subplot(2,1,2, sharex=ax1)
        librosa.display.specshow(mfcc_noise_scale, x_axis='time')
        plt.title('Noise')
        plt.tight_layout()
    
    # Chroma_stft
    elif feature == 'chroma':
        chroma_uav = librosa.feature.chroma_stft(y_uav, sr=sr_uav)
        chroma_uav_scale = scale(chroma_uav, axis=1)
        chroma_noise = librosa.feature.chroma_stft(y_noise, sr=sr_noise)
        chroma_noise_scale = scale(chroma_noise, axis=1)
        
        plt.figure(figsize=(14, 9))
        ax1 = plt.subplot(2,1,1)
        librosa.display.specshow(chroma_uav_scale, x_axis='time')
        plt.title('UAV')
        ax2 = plt.subplot(2,1,2, sharex=ax1)
        librosa.display.specshow(chroma_noise_scale, x_axis='time')
        plt.title('Noise')
        plt.tight_layout()
    
    else:
        print("Wrong input")
    
if __name__ == '__main__':
    # visualize audio's feature
    #visualize_feature('mfcc')
    
    # Convert model file to .tflite
    convert_func()
    




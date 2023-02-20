## Machine Learning part  
   
* Extract features of UAV data.
* Train machine learning models and compare performances.
* Return classification results to server.   

```bash
├── functions
│   ├── function_test.py
│   └── processing_func.py
├── save
│   ├── mfcc_3.pkl
│   └── svm_model.pkl
├── server
│   └── server_main.py
├── model_training.py
└── module.py
``` 

## Dependencies   
```
Python 3.9.5
Anaconda 22.9.0
Spyder 5.2.2 (Editor)

tensorflow 2.10.0
scikit-learn 1.0.2
numpy 1.24.2
pandas 1.5.3
matplotlib 3.5.2
joblib 1.1.0
librosa 0.9.2
 - h5py 3.8.0
 - werkzeug 2.2.2
 - markdown 3.4.1
```   

## To run this code
* Download Object_Detection folder   
* Copy and paste files in your working directory.
### Linux

```bash
# Install Anaconda
$ wget https://repo.anaconda.com/archive/Anaconda3-2022.10-Linux-x86_64.sh   
$ bash 'Anaconda3-2022.10-Linux-x86_64.sh'

# Run Anaconda
$ source ~/.bashrc
or 
$ . ~/.bashrc

# Make virtual environment   
conda create -n uavml python==3.9.5  
conda activate uavml   
sudo apt-get install libsndfile1   
sudo apt-get install make

# Run training code   
make env
make train
```


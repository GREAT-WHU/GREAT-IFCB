# GREAT-IFCB: An open-source software for multi-GNSS inter-frequency clock bias estimation

## PythonScripts

Python scripts for Download OBS/NAV/DCB Data, IFCB Estimation and draw IFCB results.
Python 3.* version is needed. The third parties `numpy` and `matplotlib` are used in the python scripts.

* Insatll dependency packages 
The file `requirements.txt` list all the dependencies for these python scripts
Type the cmmand for installing the dependency packages:
```
pip3 install -r requirements.txt
```

* Download OBS
Type the command for more info
```
python3 download_obs.py --help
```

* Download NAV
Type the command for more info
```
python3 download_nav.py --help
```

* Donwnload DCB
Type the command for more info
```
python3 download_dcb.py --help
```

* Batch process for IFCB estimation
The file `ifcb_example.ini` is the example configure file for `ifcb.py` 
Type the command for using the `ifcb.py`
```
python3 ifcb.py ifcb_example.ini
```

* Draw IFCB results
Type the command for more info
```
python3 draw_ifcb.py --help
python3 draw_rms_ifcb.py --help
```

  

#!/bin/bash


gcc ./src/host.c -o ./src/host `pkg-config --cflags --libs glib-2.0` -lpthread 

gcc ./src/KVclient.c -o ./src/KVclient `pkg-config --cflags --libs glib-2.0` -lpthread



#!/bin/bash

gcc host.c `pkg-config --cflags --libs glib-2.0` -lpthread

gcc KVclient.c `pkg-config --cflags --libs glib-2.0` -lpthread



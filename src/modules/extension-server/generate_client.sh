#!/bin/bash

openapi-generator generate -i server_spec.yaml -g cpp-qt-client --additional-properties=cppNamespace=RespExtServer  -o .

#!/bin/bash

bash compile-proto.sh; (( exit_status = exit_status || $? ))
bash build-tests.sh; (( exit_status = exit_status || $? ))
bash run-tests.sh; (( exit_status = exit_status || $? ))
bash build-firmware.sh; (( exit_status = exit_status || $? ))

exit $exit_status
while true; do find src tests | entr -d bash test.sh; done

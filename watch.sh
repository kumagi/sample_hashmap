watchmedo shell-command --patterns="*pp" --recursive --wait --command="cd build && ninja -j 8 > build.log 2>&1 >> build.log 2>&1 && notify-send BUILD-success || notify-send Failed."

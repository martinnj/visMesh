#!/bin/bash

# get the current path
CURPATH=`pwd`

inotifywait -mqr --timefmt '%d/%m/%y %H:%M' --format '%T %w %f' \
-e close_write ./ | while read date time dir file; do
  ext="${file##*.}"
  if [ "$ext" = "tex" ]; then
    FILECHANGE=${dir}${file}
    # convert absolute path to relative
    FILECHANGEREL=`echo "$FILECHANGE" | sed 's_'$CURPATH'/__'`

    echo "At ${time} on ${date}, file $FILECHANGE was changed. Rebuilding."

    # Actual command to execute when CSS or JS file is changed
    pdflatex -halt-on-error -file-line-error ${file};
  fi
done
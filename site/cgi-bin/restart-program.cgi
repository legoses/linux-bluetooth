#!/bin/bash

root_dir="../../"

#./root_dir/build/dbus-ble &
echo "Content-type: text/html"
echo ""
../build/dbus-ble &
echo '<META HTTP-EQUIV=Refresh CONTENT="0; URL=http://localhost:8000">'

#echo '<html>'
#echo '<head>'
#echo '<title>Site</title>'
#echo '</head>'
#echo '<body>'
#echo $(pwd)
#echo '</body>'
#echo '</html>'

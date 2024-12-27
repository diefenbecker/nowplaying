<?php
function getFhemReading($readingName, $fhemUrl) {
    $fhemCmd = "list $readingName";
    $cmd = "curl -s \"$fhemUrl/fhem?cmd=".urlencode($fhemCmd)."&XHR=1\"";    
    $output = shell_exec($cmd);
    $value = null;
    $match=explode(" ", $output);
    $match=array_filter($match);
    return str_replace(array("\r","\n"),'',end($match));
}

$fhemUrl = "http://<url_zum_fhem>:<fhem_port>";

$aussentemperatur = round(getFhemReading("Aussenthermometer temperature", $fhemUrl),1);
$akkustand = round(getFhemReading("MQTT2_openWB_Client SOC",$fhemUrl),1);
$pvleistung = round(getFhemReading("MQTT2_openWB_Client SolarPower",$fhemUrl)/1000,1);
$pufferoben = round(getFhemReading("HeizungPufferOben state",$fhemUrl),1);
$pufferunten = round(getFhemReading("HeizungPufferUnten state",$fhemUrl),1);

$cmd = "curl -s \"http://<ip_nodemcu>/receivedata?aussentemperatur=".$aussentemperatur."&akkustand=".$akkustand."&pvleistung=".$pvleistung."&pufferoben=".$pufferoben."&pufferunten=".$pufferunten."\"";

shell_exec($cmd);
?>
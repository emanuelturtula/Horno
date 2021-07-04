let url = 'http://pihole.lan.com:8000/horno/';

var t = setInterval(updateData, 1000);
var temp = document.getElementById("temp");
var voltage = document.getElementById("voltage");

async function updateData() {
    let response = await fetch(url, {mode: 'cors'});
    let data = await response.text();
    let parsedData = JSON.parse(data);
    voltage.innerHTML = parsedData.voltage    
    if (parsedData.temperature >= 80)
    {
        temp.innerHTML = parsedData.temperature;
    }
    else
    {
        temp.innerHTML = "Temperatura menor a 80 °"
    }

}


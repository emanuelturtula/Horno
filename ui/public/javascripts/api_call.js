let url = 'http://pihole.lan.com:8000/horno/';

var t = setInterval(fetchData,1000);

async function fetchData() {
    let response = await fetch(url, {mode: 'cors'});
    let data = await response.text();
    let parsedData = JSON.parse(data);
    console.log(parsedData);
    var temp = document.getElementById("temp");
    if (parsedData.temperature >= 80)
    {
        temp.innerHTML = parsedData.temperature;
    }
    else
    {
        temp.innerHTML = "Temperatura menor a 80 °"
    }
}


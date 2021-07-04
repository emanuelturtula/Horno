let url = 'http://pihole.lan.com:8000/horno/';

var t = setInterval(fetchData,1000);

async function fetchData() {
    let response = await fetch(url, {mode: 'cors'});
    let data = await response.text();
    let parsedData = JSON.parse(data);
    console.log(parsedData);
    var temp = document.getElementById("temp");
    temp.innerHTML = parsedData.temperature
}


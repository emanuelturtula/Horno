let url = 'http://192.168.0.2:3000/horno/';

var t = setInterval(fetchData,1000);

async function fetchData() {
    let response = await fetch(url, {mode: 'cors'});
    let data = await response.text();
    let parsedData = JSON.parse(data);
    console.log(parsedData);
    var temp = document.getElementById("temp");
    temp.innerHTML = parsedData.temperature
}


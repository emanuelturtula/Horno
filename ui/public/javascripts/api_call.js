let url = 'http://192.168.0.2:3000/horno/';

fetchText();

async function fetchText() {
    let response = await fetch(url, {mode: 'cors'});
    let data = await response.text();
    console.log(data);
}

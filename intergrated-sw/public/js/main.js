function click() {
    document.getElementById('counter').innerHTML = Number(document.getElementById('counter').innerHTML) + 1;
}

button1 = document.getElementById('button1');
button1.onclick = function() {
    click();
}

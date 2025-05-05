fetch('http://127.0.0.1:18080/api/getClassrooms', {
    method: 'GET',
    headers: {
        'Accept': 'application/json'
    }
}).then(res => res.json())
    .then(data => {
        let building = '';
        for (const room of data['room_numbers']) {
            building = building + '<div' + ' id = '+'floor'+room + '>' + room + '</div>';
        }
        document.getElementById('building').innerHTML = building;
    })
    .catch(err => {
        console.error('出错...', err);
    });
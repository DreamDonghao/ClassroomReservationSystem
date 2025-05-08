let user_id = 0;
fetch('http://127.0.0.1:18080/api/isLogin', {
    method: 'GET',
    credentials: "include",
    headers: {
        'Accept': 'application/json'
    }
}).then(res => res.json())
    .then(data => {
        if(data['isLogin']){
            user_id = data["user_id"];
            console.log(true,user_id);
        }else{
            console.log(data['isLogin']);
           user_id = 0;
        }
        document.getElementById("username").innerText = user_id;
    });

// fetch('http://127.0.0.1:18080/api/getUserName', {
//     method: 'GET',
//     headers: {
//         'Accept': 'application/json'
//     }
// }).then(res => res.json())
//     .then(data => {
//         if(data['isLogin'] === true ){
//             user_id = data["user_id"];
//         }else{
//             user_id = 0;
//         }
//     });


const floorsContainer = document.getElementById('floorsContainer');
const modal = document.getElementById('modal');
const modalTitle = document.getElementById('modalTitle');
const modalContent = document.getElementById('modalContent');
const closeModal = document.getElementById('closeModal');


async function reserveClassroom(classroomId, time_period) {
    const today = new Date();
    const tomorrow = new Date(today);
    tomorrow.setDate(today.getDate() + 1);

    const response = await fetch('/api/reserveClassroom', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            "classroom_id": classroomId,
            "user_id": 1,
            "year": tomorrow.getFullYear(),
            "month": tomorrow.getMonth() + 1,
            "day": tomorrow.getDate(),
            "time_period": time_period,
        })
    });

    return await response.json();
}


async function f(classroom_id, time_period, building, floor, classroom,) {
    const confirmButton = document.createElement('div');
    confirmButton.textContent = '确定';
    confirmButton.onclick = async function () {
        console.log(classroom_id, time_period);
        let a = await reserveClassroom(classroom_id, time_period);
        if (a['success']) {
            console.log('yes');
        } else {
            console.log('no');
        }
    };

    const cancelButton = document.createElement('div');
    cancelButton.textContent = '取消';
    cancelButton.onclick = function () {
        modal.style.display = 'none';
    };

    modalContent.innerHTML = `
            <strong>楼栋：</strong>${building}号楼<br>
            <strong>楼层：</strong>${floor}层<br>
            <strong>教室：</strong>${classroom}<br>
            `;
    modalContent.appendChild(confirmButton);
    modalContent.appendChild(cancelButton);
    modal.style.display = 'flex';
}

// 加载数据
fetch('http://127.0.0.1:18080/api/getClassrooms', {
    method: 'GET',
    headers: {
        'Accept': 'application/json'
    }
}).then(res => res.json())
    .then(data => {
        const buildingMap = {};
        data['classrooms'].forEach(classroom => {
            if (!buildingMap[classroom['buildingNumber']]) buildingMap[classroom['buildingNumber']] = {};
            if (!buildingMap[classroom['buildingNumber']][classroom['floorNumber']]) {
                buildingMap[classroom['buildingNumber']][classroom['floorNumber']] = [];
            }
            buildingMap[classroom['buildingNumber']][classroom['floorNumber']].push(classroom);
        });
        Object.entries(buildingMap).forEach(([building, floors]) => {
            const buildingDiv = document.createElement('div');
            buildingDiv.className = 'building-section';
            buildingDiv.innerHTML = `<h2 class="building-title"> ${building}号楼</h2>`;
            Object.entries(floors).forEach(([floor, classrooms]) => {
                const floorDiv = document.createElement('div');
                floorDiv.className = 'floor-section';
                floorDiv.innerHTML = `<div class="floor-title"> 第 ${floor} 层</div>`;
                const grid = document.createElement('div');
                grid.className = 'classroom-grid';
                classrooms.forEach(classroom => {
                    const card = document.createElement('div');
                    card.className = 'classroom-card';
                    card.innerHTML = `${classroom['classroomNumber']}
                            <div class = 'time_period' onclick=f(${classroom['id']},'8:30-10:05',${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})>
                            8:30-10:05</div>
                            <div class = 'time_period'  onclick="f(${classroom['id']},'10:25-12:00',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            10:25-12:00</div>
                            <div class = 'time_period' onclick="f(${classroom['id']},'14:00-15:35',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            14:00-15:35</div>
                            <div class = 'time_period' onclick="f(${classroom['id']},'15:55-17:30',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            15:55-17:30</div>
                            <div class = 'time_period' onclick="f(${classroom['id']},'18:30-20:05',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            18:30-20:05</div>`;
                    grid.appendChild(card);
                });
                floorDiv.appendChild(grid);
                buildingDiv.appendChild(floorDiv);
            });
            floorsContainer.appendChild(buildingDiv);
        });
    })
    .catch(err => {
        console.error("教室数据加载失败：", err);
        floorsContainer.innerHTML = "<p>加载失败,服务器可能在摸鱼</p>";
    });

// 弹窗关闭逻辑
closeModal.onclick = () => modal.style.display = 'none';
modal.onclick = (e) => {
    if (e.target === modal) modal.style.display = 'none';
};

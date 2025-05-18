//baseUrl = `//http://47.93.221.71:18080;`

// 判断是否已经登录，没有登录返回登录界面，登录则获取用户信息
let user_id, user_student_number, user_name;

async function determineLogin() {
    await fetch(`/api/isLogin`, {
        method: 'GET', credentials: "include", headers: {
            'Accept': 'application/json'
        }
    }).then(res => res.json())
        .then(data => {
            // 成功登录
            if (data['isLogin']) {
                document.getElementById("login_message").innerText = "退出登录";
                // 用户在数据库中的id
                user_id = data["user_id"];
                // 学号
                user_student_number = data["student_number"];
                // 姓名
                user_name = data["username"];
            }
            // 没有登录
            else {
                window.location.href = "/login"
            }
        })
}


// 今天日期
const today = new Date();

// 明天日期
const one_after_day = new Date(today);
one_after_day.setDate(today.getDate() + 1);

// 后天日期
const two_after_day = new Date(one_after_day);
two_after_day.setDate(one_after_day.getDate() + 1);

// 大后天日期
const three_after_day = new Date(two_after_day);
three_after_day.setDate(two_after_day.getDate() + 1);

// 当前时间
function getdds(day) {
    const year = day.getFullYear();
    const month = day.getMonth() + 1;
    const date = day.getDate();
    return {"year": year.toString(), "month": month.toString(), "date": date.toString()}
}

// 初次加载界面
async function loadPage() {
    await determineLogin();
    // 渲染姓名学号
    document.getElementById("username").innerText = user_name;
    document.getElementById("user_student_number").innerText = user_student_number;
    // 渲染时间及选择日期
    document.getElementById("now_time").innerText = `您可预约未来3天的教室,当前时间:${getdds(today).year}-${getdds(today).month}-${getdds(today).date}`;
    document.getElementById("choose-one").innerText = `${getdds(one_after_day).month}-${getdds(one_after_day).date}`;
    document.getElementById("choose-two").innerText = `${getdds(two_after_day).month}-${getdds(two_after_day).date}`;
    document.getElementById("choose-three").innerText = `${getdds(three_after_day).month}-${getdds(three_after_day).date}`;

}

loadPage();

const classroom_id_time_period = {};
const classroom_username = {};

// 选择预约日期
async function getSelected(day) {
    switch (day) {
        case 1:
            window.choose_day = one_after_day;
            break;
        case 2:
            window.choose_day = two_after_day;
            break;
        case 3:
            window.choose_day = three_after_day;
            break;
    }
    document.getElementById("choose_day").innerText = "您正在预约:" + (choose_day.getMonth() + 1).toString() + "-" + choose_day.getDate().toString();
    await renderingClassroom(choose_day, 1, 1);
    await getReservationClassroom();
}


const floorsContainer = document.getElementById('floorsContainer');
const modal = document.getElementById('modal');
const modalTitle = document.getElementById('modalTitle');
const modalContent = document.getElementById('modalContent');
const closeModal = document.getElementById('closeModal');

// 预约教室
async function reserveClassroom(classroomId, time_period) {
    const response = await fetch('/api/reserveClassroom', {
        method: 'POST', headers: {
            'Content-Type': 'application/json'
        }, body: JSON.stringify({
            "classroom_id": classroomId,
            "username": user_name,
            "user_id": user_id,
            "year": choose_day.getFullYear(),
            "month": choose_day.getMonth() + 1,
            "day": choose_day.getDate(),
            "time_period": time_period,
        })
    });

    return await response.json();
}


async function showModal(classroom_id, time_period, building, floor, classroom,) {
    const confirmButton = document.createElement('div');
    confirmButton.style.background = "#e5fffb"
    confirmButton.textContent = '确定预约';
    confirmButton.onclick = async function () {
        console.log(classroom_id, time_period);
        let a = await reserveClassroom(classroom_id, time_period);
        close_modal();
    };

    const cancelButton = document.createElement('div');
    cancelButton.style.background = "#ffdada"
    cancelButton.textContent = '取消预约';

    cancelButton.onclick = function () {
        close_modal();
    };

    modalContent.innerHTML = `
            <strong>楼栋：</strong>${building}号楼<br>
            <strong>楼层：</strong>${floor}层<br>
            <strong>教室：</strong>${classroom}<br>
            <strong>时间：</strong>${choose_day.getMonth().toString() + "." + choose_day.getDate().toString() + ":" + time_period}<br>
            <strong id = "reservations_username">当前预约:无人预约</strong>
            `;
    modalContent.appendChild(confirmButton);
    modalContent.appendChild(cancelButton);
    modal.style.display = 'flex';
    if (classroom_username[choose_day.toString() + classroom_id + time_period]) {
        document.getElementById("reservations_username").innerText = "当前预约:" + classroom_username[choose_day.toString() + classroom_id + time_period];
    }
}


async function getReservationClassroom() {
    const response = await fetch('/api/get_reservations_classroom_id', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            "year": window.choose_day.getFullYear(),
            "month": window.choose_day.getMonth() + 1,
            "date": window.choose_day.getDate(),
        })
    });

    const data = await response.json(); // 关键的一步！

    let orc = [];
    let mrc = [];

    data["reservations"].forEach(item => {
        const id = choose_day.toString() + item["classroom_id"] + item["time_period"];
        if (item["user_id"] === user_id) {
            mrc.push(id);
        } else {
            orc.push(id);
        }
        classroom_username[id] = item["username"];
    });

    // 用户自己的预约标红
    mrc.forEach(itemId => {
        const el = document.getElementById(itemId);
        console.log(el);
        if (el) el.style.background = "#86f8d0";
    });

    // 其他人的预约（比如标灰？）
    orc.forEach(itemId => {
        const el = document.getElementById(itemId);
        if (el) el.style.background = "#ff5f5f";
    });
}


async function getAllClassroom() {
    try {
        const res = await fetch('/api/getClassrooms', {
            method: 'GET',
            headers: {
                'Accept': 'application/json'
            }
        });
        return await res.json();
    } catch (err) {
        console.error("教室数据加载失败：", err);
        floorsContainer.innerHTML = "<p>加载失败,服务器可能在摸鱼</p>";
        return null;
    }
}


// 渲染网页教室
async function renderingClassroom(now_day, other_reserved_classroom, my_reserved_classroom) {
    floorsContainer.innerHTML = ""; // 清空之前的楼层内容
    let data = await getAllClassroom();
    console.log(data);
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
                            <div id = "${choose_day.toString()}${classroom["id"]}8:30-10:05" class = 'time_period' onclick="showModal(${classroom['id']},'8:30-10:05',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            8:30-10:05</div>
                            <div id = "${choose_day.toString()}${classroom["id"]}10:25-12:00" class = 'time_period'  onclick="showModal(${classroom['id']},'10:25-12:00',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            10:25-12:00</div>
                            <div id = "${choose_day.toString()}${classroom["id"]}14:00-15:35" class = 'time_period' onclick="showModal(${classroom['id']},'14:00-15:35',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            14:00-15:35</div>
                            <div id = "${choose_day.toString()}${classroom["id"]}15:55-17:30" class = 'time_period' onclick="showModal(${classroom['id']},'15:55-17:30',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            15:55-17:30</div>
                            <div id = "${choose_day.toString()}${classroom["id"]}18:30-20:05" class = 'time_period' onclick="showModal(${classroom['id']},'18:30-20:05',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            18:30-20:05</div>`;

                grid.appendChild(card);
            });
            floorDiv.appendChild(grid);
            buildingDiv.appendChild(floorDiv);
        });
        floorsContainer.appendChild(buildingDiv);

    });

}

// 弹窗关闭逻辑
closeModal.onclick = () => {
    close_modal();
}
modal.onclick = (e) => {
    if (e.target === modal) modal.style.display = 'none';
}

function close_modal() {
    modal.style.display = 'none';
    getReservationClassroom();
}



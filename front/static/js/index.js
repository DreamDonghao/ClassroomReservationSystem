baseUrl = "http://127.0.0.1:18080;"

// 判断是否已经登录，没有登录返回登录界面，登录则获取用户信息
let user_id, user_student_number, user_name;

async function determineLogin() {
    await fetch(`http://127.0.0.1:18080/api/isLogin`, {
        method: 'GET', credentials: "include", headers: {
            'Accept': 'application/json'
        }
    }).then(res => res.json())
        .then(data => {
            // 成功登录
            if (data['isLogin']) {
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
    document.getElementById("now_time").innerText = `当前时间:${getdds(today).year}-${getdds(today).month}-${getdds(today).date}`;
    document.querySelector("#choose-one .day-label").innerText = `${getdds(one_after_day).month}-${getdds(one_after_day).date}`;
    document.querySelector("#choose-two .day-label").innerText = `${getdds(two_after_day).month}-${getdds(two_after_day).date}`;
    document.querySelector("#choose-three .day-label").innerText = `${getdds(three_after_day).month}-${getdds(three_after_day).date}`;
}

loadPage();

const classroom_id_time_period = {};
const classroom_username = {};

// 选择预约日期
async function getSelected() {
    document.querySelectorAll(".time_period").forEach(item => {
        item.style.background = "#fff";
    });
    const selected = document.querySelector('input[name="day"]:checked');
    if (selected) {
        switch (selected.value) {
            case "one":
                window.choose_day = one_after_day;
                break;
            case "two":
                window.choose_day = two_after_day;
                break;
            case "three":
                window.choose_day = three_after_day;
                break;
        }

        await fetch('http://127.0.0.1:18080/api/get_reservations_classroom_id', {
            method: 'POST', headers: {
                'Content-Type': 'application/json'
            }, body: JSON.stringify({
                "year": choose_day.getFullYear(), "month": choose_day.getMonth() + 1, "date": choose_day.getDate(),
            })
        }).then(res => res.json())
            .then(data => {
                data["reservations"].forEach(item => {
                    document.getElementById(item["classroom_id"] + "+" + item["time_period"]).style.background = "#ff0000";
                    if (!classroom_id_time_period[item["classroom_id"]]) {
                        classroom_id_time_period[item["classroom_id"]] = [];
                    }
                    classroom_id_time_period[item["classroom_id"]].push(item["time_period"]);
                    classroom_username[item["classroom_id"] + "+" + item["time_period"]] = item["username"];
                    console.log(classroom_username);
                })
                fetch('http://127.0.0.1:18080/api/getClassrooms', {
                    method: 'GET', headers: {
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
                            <div id = "${classroom["id"]}+8:30-10:05" class = 'time_period' onclick="f(${classroom['id']},'8:30-10:05',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            8:30-10:05</div>
                            <div id = "${classroom["id"]}+10:25-12:00" class = 'time_period'  onclick="f(${classroom['id']},'10:25-12:00',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            10:25-12:00</div>
                            <div id = "${classroom["id"]}+14:00-15:35" class = 'time_period' onclick="f(${classroom['id']},'14:00-15:35',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            14:00-15:35</div>
                            <div id = "${classroom["id"]}+15:55-17:30" class = 'time_period' onclick="f(${classroom['id']},'15:55-17:30',
                            ${classroom['buildingNumber']},${classroom['floorNumber']},${classroom['classroomNumber']})">
                            15:55-17:30</div>
                            <div id = "${classroom["id"]}+18:30-20:05" class = 'time_period' onclick="f(${classroom['id']},'18:30-20:05',
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

            });
    } else {
        alert("你还没选呢！");
    }
}


const floorsContainer = document.getElementById('floorsContainer');
const modal = document.getElementById('modal');
const modalTitle = document.getElementById('modalTitle');
const modalContent = document.getElementById('modalContent');
const closeModal = document.getElementById('closeModal');


async function reserveClassroom(classroomId, time_period) {
    const response = await fetch('/api/reserveClassroom', {
        method: 'POST', headers: {
            'Content-Type': 'application/json'
        }, body: JSON.stringify({
            "classroom_id": classroomId,
            "username": user_name,
            "user_id": 1,
            "year": choose_day.getFullYear(),
            "month": choose_day.getMonth() + 1,
            "day": choose_day.getDate(),
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
            <strong>时间：</strong>${choose_day.getMonth().toString() + "." + choose_day.getDate().toString() + ":" + time_period}<br>
            <strong id = "reservations_username">当前预约:无人预约</strong>
            `;
    modalContent.appendChild(confirmButton);
    modalContent.appendChild(cancelButton);
    modal.style.display = 'flex';
    if (classroom_username[classroom_id + "+" + time_period]) {
        document.getElementById("reservations_username").innerText = "当前预约:" + classroom_username[classroom_id + "+" + time_period];
    }
}

// 渲染网页教室


// 弹窗关闭逻辑
closeModal.onclick = () => modal.style.display = 'none';
modal.onclick = (e) => {
    if (e.target === modal) modal.style.display = 'none';
};

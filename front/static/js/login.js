document.getElementById('loginForm').addEventListener('submit', async (e) => {
    e.preventDefault(); // 阻止表单默认提交行为

    // 获取输入值
    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;
    try {
        const response = await fetch('/api/login', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    studentNumber: username,
                    password: password
                })
            }
        );
        const data = await response.json();
        if (data['success']) {
            setTimeout(
                () => {
                    window.location.href = '/index';  // 跳转到目标页面，
                }, 1000
            );
        }
    } catch (error) {

    }
})
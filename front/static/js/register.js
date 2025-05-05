document.getElementById('registerForm').addEventListener('submit', async (e) => {
    e.preventDefault();

    const username = document.getElementById('username').value;
    const studentNumber = document.getElementById('studentNumber').value;
    const password = document.getElementById('password').value;
    const confirmPassword = document.getElementById('confirmPassword').value;

    if (password !== confirmPassword) {
        document.getElementById('message').innerText = "两次密码不一致";
        return;
    }

    try {
        const response = await fetch('/api/register', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ username,studentNumber, password,confirmPassword })
        });
        const data = await response.json();
        if (data.success){
            document.getElementById("message").innerText = '您的账号为\n'+data.message;
            setTimeout(() => {
                window.location.href = '/login'; // 直接跳转
            }, 1000);
        } else {
            document.getElementById('message').innerText = data.message;
        }
    } catch (error) {
        document.getElementById('message').innerText = error.message;
        console.error('注册失败:', error);
    }
});
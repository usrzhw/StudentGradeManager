var user_name;

window.onload = function () {
    if (localStorage.getItem("permission") != 0) {
        //不是管理员账户，跳转普通用户登录
        jump_to_login();
    }
    var token = localStorage.getItem("token");
    if (token == "") location.href = "login.html";
    //检查token是否有效
    fetch("/app/stu.get_user_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: token,
                target_id: localStorage.getItem("id")
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            if (json_data.permission_level != localStorage.getItem("permission")) {
                throw new Error("permission changed");
            }
        }).catch(error => {
            jump_to_login();
        });
}
function jump_to_login() {
    location.href = "login.html";
}
function logout() {
    var id = localStorage.getItem("id");
    var token = localStorage.getItem("token");
    localStorage.removeItem("id");
    localStorage.removeItem("token");
    localStorage.removeItem("permission");
    fetch("/app/stu.logout",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: id,
                token: token,
                target_id: id
            }),
            mode: 'no-cors'
        }).then(Response => {
            jump_to_login();
        }).catch(error => {
            jump_to_login();
        });
}

function on_item_click(e) {
    var frame = document.getElementById("MainFrame");
    if (e.text == "学生管理")
        frame.src = "adminstu.html";
    else if (e.text == "教师管理")
        frame.src = "admintec.html";
    else if (e.text == "课程管理")
        frame.src = "tecsubjects.html";
    else if (e.text == "账号管理")
        frame.src = "self_account.html";
    var list = document.querySelectorAll("li a");
    for (var it of list) {
        if (it.textContent == e.textContent) {
            it.classList.add("active");
        }
        else it.classList.remove("active");
    }
}

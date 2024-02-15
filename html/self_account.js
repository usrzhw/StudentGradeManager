
window.onload = async function () {
    await fetch("/app/stu.get_user_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
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

            var str = "" +
                "<div class=\"item\">" +"姓名: "+ json_data.name + "</div>" +
                "<div class=\"item\">" +"学号: "+ localStorage.getItem("id") + "</div>" +
                "<div class=\"item\">" +"性别: "+ json_data.sex + "</div>" +
                "<div class=\"item\">" +"学院: "+ json_data.college + "</div>";
            if (json_data.type == "student") {
                str += "<div class=\"item\">" +"班级: "+ json_data.class_name + "</div>";
                str += "<div class=\"item\">" +"入学年份: "+ json_data.enrollment_date + "</div>"
            }
            document.getElementById("info_bar").innerHTML = str + document.getElementById("info_bar").innerHTML;
        }).catch(error => {
        });
}

function change_password() {
    var favDialog = document.getElementById("ChangePasswordDialog");
    if (typeof favDialog.showModal === "function") {
        favDialog.showModal();
    } else {
        alert("浏览器不支持，请使用最新版Chrome浏览器");
    }
    
}
async function make_sure_change() {
    var old_pwd = document.getElementById("old_password").value;
    var new_pwd = document.getElementById("new_password").value;
    if (new_pwd != document.getElementById("re_new_password").value) {
        alert("两次密码输入不同");
        cancel_change();
        return;
    }
    else if (new_pwd == "") {
        alert("密码不可设置为空");
        cancel_change();
        return;
    }
    await fetch("/app/stu.change_password",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                target_id: localStorage.getItem("id"),
                password: new_pwd,
                old_password: old_pwd

            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            if (json_data.message == "ok") {
                alert("修改成功，请重新登陆以生效");
            }
        }).catch(error => {
            alert("修改失败");
        });
    cancel_change();
}

function cancel_change() {
    document.getElementById("old_password").value = "";
    document.getElementById("new_password").value = "";
    document.getElementById("re_new_password").value = "";
    var favDialog = document.getElementById("ChangePasswordDialog");
    favDialog.close();
}

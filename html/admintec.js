
var opened_teacher_info;
var opened_teacher_id;
window.onload = ShowTeachersList();

function up(x, y) {
    return Number(x.id) - Number(y.id);
}
async function RequestJson(url, body) {
    return fetch(url,
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: body,
            mode: 'no-cors'
        }).then(async Response => {
            if (Response.ok == false) {
                throw new Error((await Response.json()).message);
            }
            else {
                return Response.json();
            }
        });
}
async function ShowTeachersList() {
    var teachers_info = await RequestJson("/app/stu.get_all_teachers_info", JSON.stringify({
        ID: localStorage.getItem("id"),
        token: localStorage.getItem("token"),
    }));
    var form = document.getElementById("FormBody");
    form.innerHTML = "";
    var str = "";
    teachers_info.teachers.sort(up);
    for (var it of teachers_info.teachers) {
        str += "<tr onclick=\"ItemClicked(this)\">" +
            "<td>" + it.id + "</td>" +
            "<td>" + it.name + "</td>" +
            "<td>" + it.sex + "</td>" +
            "<td>" + it.college + "</td>" +
            "<td>" + it.notes + "</td > " +
            "</tr>";
    }
    form.innerHTML = str;
}

function ItemClicked(e) {
    //获取被点击的元素对应的学号
    var TeacherID = e.firstElementChild.textContent;
    OpenTeacherInfoDialog(TeacherID);
}

async function OpenTeacherInfoDialog(teacher_id) {
    var teacher_info = await RequestJson("/app/stu.get_user_info", JSON.stringify({
        ID: localStorage.getItem("id"),
        token: localStorage.getItem("token"),
        target_id: teacher_id
    }));
    //设置dialog中各个属性的值
    document.getElementById("NameBox").value = teacher_info.name;
    document.getElementById("TeacherIDBox").value = teacher_id;
    document.getElementById("SexBox").value = teacher_info.sex;
    document.getElementById("PhoneNumberBox").value = teacher_info.phone_number;
    document.getElementById("EMailBox").value = teacher_info.email;
    document.getElementById("CollegeBox").value = teacher_info.college;
    document.getElementById("NotesBox").value = teacher_info.notes;
    document.getElementById("PermissionLevelBox").value = teacher_info.permission_level;
    opened_teacher_info = teacher_info;
    opened_teacher_id = teacher_id;
    document.getElementById("TeacherInfo").showModal();
}

function Close() {
    document.getElementById("TeacherInfo").close();
    document.getElementById("AddTeacherDialog").close();
}

async function Makesure() {
    //点击确定按钮后的操作
    //依次检查哪些属性被修改了
    var teacher_id = opened_teacher_id;
    if (document.getElementById("NameBox").value != opened_teacher_info.name) {
        //名称被修改
        await RequestJson("/app/stu.change_name", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: teacher_id,
            name: document.getElementById("NameBox").value
        })).catch(error => {
            alert("修改名称失败:" + error.toString());
        });
    }
    if (document.getElementById("SexBox").value != opened_teacher_info.sex) {
        await RequestJson("/app/stu.change_sex", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: teacher_id,
            sex: document.getElementById("SexBox").value
        })).catch(error => {
            alert("修改性别失败:" + error.toString());
        });
    }
    if (document.getElementById("PhoneNumberBox").value != opened_teacher_info.phone_number) {
        await RequestJson("/app/stu.change_phone_number", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: teacher_id,
            phone_number: document.getElementById("PhoneNumberBox").value
        })).catch(error => {
            alert("修改手机号码失败:" + error.toString());
        });
    }
    if (document.getElementById("EMailBox").value != opened_teacher_info.email) {
        await RequestJson("/app/stu.change_email", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: teacher_id,
            email: document.getElementById("EMailBox").value
        })).catch(error => {
            alert("修改邮箱失败:" + error.toString());
        });
    }
    if (document.getElementById("CollegeBox").value != opened_teacher_info.college) {
        await RequestJson("/app/stu.change_college", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: teacher_id,
            college: document.getElementById("CollegeBox").value
        })).catch(error => {
            alert("修改学院失败:" + error.toString());
        });
    }
    if (document.getElementById("NotesBox").value != opened_teacher_info.notes) {
        await RequestJson("/app/stu.change_notes", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: teacher_id,
            notes: document.getElementById("NotesBox").value
        })).catch(error => {
            alert("修改用户备注失败:" + error.toString());
        });
    }
    if (document.getElementById("PermissionLevelBox").value != opened_teacher_info.permission_level) {
        await RequestJson("/app/stu.change_permission_level", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: teacher_id,
            permission_level: document.getElementById("PermissionLevelBox").value
        })).catch(error => {
            alert("修改权限等级失败:" + error.toString());
        });
    }
    if (document.getElementById("PasswordBox").value != "") {
        await RequestJson("/app/stu.change_password", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: teacher_id,
            password: document.getElementById("PasswordBox").value
        })).catch(error => {
            alert("修改密码失败:" + error.toString());
        });
    }
    Close();
    ShowTeachersList();
}

function OpenAddTeacherDialog() {
    document.getElementById("ANameBox").value = "";
    document.getElementById("ASexBox").value = "";
    document.getElementById("APhoneNumberBox").value = "";
    document.getElementById("AEMailBox").value = "";
    document.getElementById("ACollegeBox").value = "";
    document.getElementById("ANotesBox").value = "";
    document.getElementById("APermissionLevelBox").value = "";
    document.getElementById("APasswordBox").value = "";
    document.getElementById("AddTeacherDialog").showModal();
}

async function CreateStudent() {
    await RequestJson("/app/stu.create_teacher", JSON.stringify({
        ID: localStorage.getItem("id"),
        token: localStorage.getItem("token"),
        name: document.getElementById("ANameBox").value,
        sex: document.getElementById("ASexBox").value,
        phone_number: document.getElementById("APhoneNumberBox").value,
        email: document.getElementById("AEMailBox").value,
        college: document.getElementById("ACollegeBox").value,
        notes: document.getElementById("ANotesBox").value,
        permission_level: document.getElementById("APermissionLevelBox").value,
        password: document.getElementById("APasswordBox").value,
    })).then(() => {
        Close();
        ShowTeachersList();
    }).catch(error => {
        alert("创建教师失败:" + error.toString());
    });
}

async function DeleteTeacher() {
    if (window.confirm("确定删除该教师吗？")) {
        await RequestJson("/app/stu.delete_teacher", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: opened_teacher_id
        })).catch(error => {
            alert("删除教师失败:" + error.toString());
        });
    }
    Close();
    ShowTeachersList();
}

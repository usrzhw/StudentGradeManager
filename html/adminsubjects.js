
window.onload = function () {
    switch_to_subjects();
}

function item_click(e) {
    if (e.textContent == "管理学生")
        switch_to_students(e.parentNode.parentNode.firstElementChild.textContent);
}

function up_sort_subjects(x, y) {
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
function down(x, y) {
    return y.grade - x.grade;
}
var last_open_student = 0;
var last_open_subject = 0;
function switch_to_subjects() {
    document.getElementById("ButtomDiv").textContent = "";
    document.getElementById("chose_bar").innerHTML = "<button onclick=\"ShowCreateSubjectDialog()\" class=\"back_button\">创建课程</button>";
    document.getElementById("FormBody").innerHTML = "";
    document.getElementById("FormHeader").innerHTML = "<tr><th>课程编号</th><th>课程名称</th></tr>"
    fetch("/app/stu.get_all_subjects",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token")
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            json_data.subjects.sort(up_sort_subjects);
            for (var it of json_data.subjects) {
                var str = "" +
                    "<tr>" +
                    "<td>" + it.id + "</td>" +
                    "<td>" + it.name + "</td>" +
                    "<td>" + "<button onclick=\"item_click(this)\" style=\"margin: 5px;\" class=\"right_button\">管理学生</button><button onclick=\"item_click(this)\" style=\"margin: 5px;\" class=\"right_button\">管理教师</button>" + "</td>" +
                    "</tr>";
                document.getElementById("FormBody").innerHTML += str;
            }
        }).catch(error => {
        });
}

function switch_to_students(subject_id) {
    document.getElementById("ButtomDiv").textContent = "";
    document.getElementById("FormBody").innerHTML = "";
    document.getElementById("FormHeader").innerHTML = "<tr><th>学号</th><th>姓名</th><th>成绩</th><th>备注</th></tr>"
    document.getElementById("chose_bar").innerHTML = "<div style=\"margin-right:auto;margin-left: 5vw;\">成绩-1表示未登记成绩</div>"
    document.getElementById("chose_bar").innerHTML = "<button onclick=\"switch_to_subjects()\" class=\"back_button\">返回</button>";
    document.getElementById("chose_bar").innerHTML += "<button onclick=\"OpenAddStudentDialog()\" class=\"back_button\">添加学生</button>";
    
    document.getElementById("chose_bar").innerHTML += "<div style=\"margin-right:auto;margin-left: 5vw;\">成绩-1表示未登记成绩</div>"
    fetch("/app/stu.get_subject_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                subject_id: subject_id
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            var sum = 0;
            var count = 0;
            json_data.students.sort(down);
            for (var it of json_data.students) {
                var str = "" +
                    "<tr onclick=\"SetStudentGrade(this)\">" +
                    "<td>" + it.id + "</td>" +
                    "<td>" + it.name + "</td>" +
                    "<td>" + it.grade + "</td>" +
                    "<td>" + it.notes + "</td>" +
                    "</tr>";
                document.getElementById("FormBody").innerHTML += str;
                if (it.grade >= 0)
                    sum += it.grade, ++count;
            }
            document.getElementById("ButtomDiv").textContent = "平均成绩: " + (sum / count);
            last_open_subject = subject_id;
        }).catch(error => {
        });
}
async function SetStudentGrade(e) {
    document.getElementById("grade_box").value = e.children[2].textContent;
    var stu_id = e.firstElementChild.textContent;
    document.getElementById("note_box").value = e.children[3].textContent;
    last_open_student = e.children[0].textContent;
    document.getElementById("SetStudentInfo").showModal();
}

async function make_sure_change() {
    await fetch("/app/stu.set_student_subject_grade",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                subject_id: last_open_subject,
                student_id: last_open_student,
                grade: document.getElementById("grade_box").value
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            if (json_data.message != "ok") throw Error(json_data.message);
        }).catch(error => {
            alert("修改失败: " + error);
        });
    document.getElementById("SetStudentInfo").close();
    switch_to_students(last_open_subject);
}
function Close() {
    document.getElementById("SetStudentInfo").close();
    document.getElementById("CreateSubjectDialog").close();
    document.getElementById("AddStudentDialog").close();
}

function ShowCreateSubjectDialog() {
    document.getElementById("AClassName").value = "";
    document.getElementById("ADescription").value = "";
    document.getElementById("AStart").value = "";
    document.getElementById("AEnd").value = "";
    document.getElementById("ASemester").value = "";
    document.getElementById("AClassRoom").value = "";
    document.getElementById("CreateSubjectDialog").showModal();
}

async function CreateSubject() {
    await RequestJson("/app/stu.create_subject", JSON.stringify({
        ID: localStorage.getItem("id"),
        token: localStorage.getItem("token"),
        start: document.getElementById("AStart").value,
        end: document.getElementById("AEnd").value,
        semester: document.getElementById("ASemester").value,
        name: document.getElementById("AClassName").value,
        class_room: document.getElementById("AClassRoom").value,
        description: document.getElementById("ADescription").value
    })).catch(error => {
        alert("创建课程失败:" + error.toString());
    });
    Close();
    switch_to_subjects();
}

function OpenAddStudentDialog() {
    document.getElementById("Agrade_box").value = "";
    document.getElementById("Anote_box").value = "";
    document.getElementById("AddStudentDialog").showModal();
}
async function AddStudent() {
    await RequestJson("/app/stu.add_member_to_subject", JSON.stringify({
        ID: localStorage.getItem("id"),
        token: localStorage.getItem("token"),
        subject_id: last_open_subject,
        member_id: document.getElementById("Agrade_box").value,
        notes: document.getElementById("Anote_box").value
    })).catch(error => {
        alert("添加学生失败:" + error.toString());
    });
    Close();
    switch_to_students(last_open_subject);
}

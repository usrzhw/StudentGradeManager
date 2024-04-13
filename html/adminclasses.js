window.onload = function () {
    switch_to_classes();
}

function item_click(e) {
    switch_to_students(e.firstElementChild.textContent);
}

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

function switch_to_classes() {
    document.getElementById("chose_bar").innerHTML = "<button onclick=\"CreateClassButton()\" class=\"back_button\">创建班级</button>";
    document.getElementById("FormBody").innerHTML = "";
    document.getElementById("FormHeader").innerHTML = "<tr><th>班级名称</th><th>班主任</th><th>创建时间</th><th>学生数量</th></tr>"
    fetch("/app/stu.get_all_classes_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            for (var it of json_data.classes) {
                var str = "" +
                    "<tr onclick=\"item_click(this)\">" +
                    "<td>" + it.class_name + "</td>" +
                    "<td>" + it.teacher_name + "</td>" +
                    "<td>" + it.create_time + "</td>" +
                    "<td>" + it.students_number + "</td>" +
                    "<td>" + "<button class=\"delete_button\" onclick=\"DeleteClassButtonClick(this)\">删除班级</button>" + "</td>" +
                    "</tr>";
                document.getElementById("FormBody").innerHTML += str;
            }
        }).catch(error => {
        });
}

function switch_to_students(class_name) {
    document.getElementById("FormBody").innerHTML = "";
    document.getElementById("FormHeader").innerHTML = "<tr><th>学号</th><th>姓名</th></tr>"
    document.getElementById("chose_bar").innerHTML = "<button onclick=\"switch_to_classes()\" class=\"back_button\">返回</button>"
    fetch("/app/stu.get_class_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                class_name: class_name
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            for (var it of json_data.students) {
                var str = "" +
                    "<tr onclick=\"ShowStudentInfo(this)\">" +
                    "<td>" + it.id + "</td>" +
                    "<td>" + it.name + "</td>" +
                    "</tr>";
                document.getElementById("FormBody").innerHTML += str;
            }
        }).catch(error => {
        });
}

async function ShowStudentInfo(e) {
    document.getElementById("GradeFormBody").innerHTML = "";
    var stu_id = e.firstElementChild.textContent;
    document.getElementById("StudentInfo").innerHTML = "";
    document.getElementById("StudentGrades").showModal();
    fetch("/app/stu.get_student_subjects",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                target_id: stu_id
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            for (var it of json_data.subjects) {
                //document.getElementById("StudentNameDiv").textContent;
                if (it.grade == -1) continue;
                var str = "" +
                    "<tr>" +
                    "<td>" + it.ID + "</td>" +
                    "<td>" + it.name + "</td>" +
                    "<td>" + it.grade + "</td>" +
                    "<td>" + it.student_note + "</td>" +
                    "</tr>";
                document.getElementById("GradeFormBody").innerHTML += str;
            }
        }).catch(error => {
        });
    fetch("/app/stu.get_user_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                target_id: stu_id

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
                "<div class=\"item\">" + "姓名: " + json_data.name + "</div>" +
                "<div class=\"item\">" + "学号: " + stu_id + "</div>" +
                "<div class=\"item\">" + "性别: " + json_data.sex + "</div>" +
                "<div class=\"item\">" + "学院: " + json_data.college + "</div>";
            document.getElementById("StudentInfo").innerHTML = str + document.getElementById("StudentInfo").innerHTML;
        }).catch(error => {
        });
}

function Close() {
    document.getElementById("AddClassDialog").close();
    document.getElementById("StudentGrades").close();
}

function CreateClassButton() {
    document.getElementById("AClassName").value = "";
    document.getElementById("ATeacherID").value = "";
    document.getElementById("AddClassDialog").showModal();
}

async function CreateClass() {
    var class_name = document.getElementById("AClassName").value;
    var teacher_id = document.getElementById("ATeacherID").value;
    var result = await RequestJson("/app/stu.create_class",
        JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            class_name: class_name,
            teacher_id: teacher_id
        })).then(() => {
            Close();
            switch_to_classes();
        }).catch(error => {
            alert(error.toString());
        });
}

function DeleteClassButtonClick(e) {
    var class_name = e.parentElement.parentElement.firstElementChild.textContent;
    if (confirm("确认删除班级" + class_name + "吗？")) {
        RequestJson("/app/stu.delete_empty_class",
            JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                class_name: class_name
            })).then(x => {
                switch_to_classes();
            }).catch(error => {
                alert(error.toString());
            });
    }
    event.stopPropagation();
}


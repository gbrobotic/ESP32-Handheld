const $=s=>document.querySelector(s), $$=s=>[...document.querySelectorAll(s)];
let base=localStorage.getItem("esp32Base")||location.origin;
let todos=[],habits=[],alarms=JSON.parse(localStorage.getItem("localAlarms")||"[]");
let timerSeconds=1500,timerRunning=false;
let focusDurationSeconds=1500;
function toast(t){const e=$("#toast");e.textContent=t;e.classList.add("show");setTimeout(()=>e.classList.remove("show"),1800)}
function api(path,opts={}){return fetch(base+path,{...opts}).then(r=>{if(!r.ok)throw Error("HTTP "+r.status);return r.json()})}
function enc(s){return encodeURIComponent(s)}
function go(page){$$(".page").forEach(x=>x.classList.toggle("active",x.id===page));$$(".nav").forEach(x=>x.classList.toggle("active",x.dataset.page===page));$("#pageTitle").textContent=page==="dashboard"?"Good day.":page[0].toUpperCase()+page.slice(1);if(page==="todo")renderTodos();if(page==="habits")renderHabits();if(page==="alarms")renderAlarms();if(page==="progress")renderProgress()}
$$(".nav").forEach(b=>b.onclick=()=>go(b.dataset.page));$$("[data-go]").forEach(b=>b.onclick=()=>go(b.dataset.go));
$("#refreshBtn").onclick=loadAll;$("#refreshDevice").onclick=loadStatus;
$("#saveUrl").onclick=async()=>{let v=$("#baseUrl").value.trim().replace(/\/$/,"");if(v){base=v;localStorage.setItem("esp32Base",v);await loadAll()}};
$("#baseUrl").value=base===location.origin?"":base;

async function loadStatus(){
 try{const d=await api("/api/status");$("#connectionDot").style.background="var(--green)";$("#connectionText").textContent="ESP32 connected";$("#pillDot").style.background="var(--green)";$("#devicePill").textContent="Connected";$("#devName").textContent=d.device||"ESP32-S3";$("#devWifi").textContent=d.wifi?"Connected":"Offline";$("#devIp").textContent=d.ip||"—";$("#devTime").textContent=d.time||"—";$("#webClock").textContent=d.time||"--:--";$("#webDate").textContent=d.date||"—";return true}
 catch(e){$("#connectionDot").style.background="var(--amber)";$("#connectionText").textContent="Device offline";$("#pillDot").style.background="var(--amber)";$("#devicePill").textContent="Offline";return false}
}
async function loadData(){
 try{todos=(await api("/api/todos")).todos||[]}catch(e){todos=[]}
 try{habits=(await api("/api/habits")).habits||[]}catch(e){habits=[]}
 renderAll()
}
async function loadAll(){const ok=await loadStatus();await loadData();if(ok)toast("Synced with ESP32")}
function renderAll(){renderTodos();renderHabits();renderDashboard();renderProgress()}
function row(item,type,index){const done=!!item.completed;return `<div class="item"><button class="check ${done?"done":""}" onclick="toggleItem('${type}',${index},${!done})">${done?"✓":""}</button><div class="item-main"><div class="item-title ${done?"done-text":""}">${escapeHtml(item.name||"Empty")}</div></div><div class="row-actions"><button class="tiny" onclick="renameItem('${type}',${index})">Edit</button><button class="tiny" onclick="deleteItem('${type}',${index})">Delete</button></div></div>`}
function renderTodos(){const html=todos.filter(x=>x.name).map((x,i)=>row(x,"todo",i)).join("")||empty("No tasks yet.");$("#todoList").innerHTML=html}
function renderHabits(){const html=habits.filter(x=>x.name).map((x,i)=>row(x,"habit",i)).join("")||empty("No habits yet.");$("#habitList").innerHTML=html}
function empty(t){return `<div class="panel"><span class="muted">${t}</span></div>`}
function renderDashboard(){let t=todos.filter(x=>x.name),h=habits.filter(x=>x.name),tc=t.filter(x=>x.completed).length,hc=h.filter(x=>x.completed).length,total=t.length+h.length,done=tc+hc,p=total?Math.round(done/total*100):0;$("#dashTasks").textContent=`${tc} / ${t.length}`;$("#dashHabits").textContent=`${hc} / ${h.length}`;$("#dashPercent").textContent=p+"%";$("#dashTodoList").innerHTML=t.slice(0,4).map((x,i)=>`<div class="list-row"><button class="check ${x.completed?"done":""}" onclick="toggleItem('todo',${todos.indexOf(x)},${!x.completed})">${x.completed?"✓":""}</button><span class="${x.completed?"done-text":""}">${escapeHtml(x.name)}</span></div>`).join("")||`<span class="muted">No tasks yet.</span>`;$("#dashHabitList").innerHTML=h.slice(0,4).map(x=>`<div class="list-row"><button class="check ${x.completed?"done":""}" onclick="toggleItem('habit',${habits.indexOf(x)},${!x.completed})">${x.completed?"✓":""}</button><span class="${x.completed?"done-text":""}">${escapeHtml(x.name)}</span></div>`).join("")||`<span class="muted">No habits yet.</span>`;$("#dashFocus").textContent=formatTime(timerSeconds)}
function renderProgress(){let t=todos.filter(x=>x.name),h=habits.filter(x=>x.name),tc=t.filter(x=>x.completed).length,hc=h.filter(x=>x.completed).length,tp=t.length?Math.round(tc/t.length*100):0,hp=h.length?Math.round(hc/h.length*100):0,total=t.length+h.length,p=total?Math.round((tc+hc)/total*100):0;$("#progressPercent").textContent=p+"%";$("#taskProgress").textContent=`${tc} / ${t.length}`;$("#taskPct").textContent=tp+"%";$("#habitProgress").textContent=`${hc} / ${h.length}`;$("#habitPct").textContent=hp+"%";$("#taskBar").style.width=tp+"%";$("#habitBar").style.width=hp+"%";document.querySelector(".ring").style.background=`conic-gradient(var(--accent) ${p*3.6}deg,#27313c 0deg)`}
async function toggleItem(type,index,value){let path=type==="todo"?"/api/todo":"/api/habit";try{await api(path,{method:"POST",headers:{"Content-Type":"application/x-www-form-urlencoded"},body:`index=${index}&completed=${value}`});await loadData();toast("Updated")}catch(e){toast("Could not reach ESP32")}}
async function renameItem(type,index){let current=(type==="todo"?todos:habits)[index]?.name||"";let name=prompt("Name",current);if(!name||name===current)return;let path=type==="todo"?"/api/todo/edit":"/api/habit/edit";try{await api(path,{method:"POST",headers:{"Content-Type":"application/x-www-form-urlencoded"},body:`index=${index}&name=${enc(name)}`});await loadData();toast("Saved")}catch(e){toast("Could not reach ESP32")}}
async function deleteItem(type,index){if(!confirm("Delete this item?"))return;let path=type==="todo"?"/api/todo/delete":"/api/habit/delete";try{await api(path,{method:"POST",headers:{"Content-Type":"application/x-www-form-urlencoded"},body:`index=${index}`});await loadData();toast("Deleted")}catch(e){toast("Could not reach ESP32")}}
async function createItem(type){let name=prompt(type==="todo"?"New task":"New habit");if(!name)return;let path=type==="todo"?"/api/todo/create":"/api/habit/create";try{await api(path,{method:"POST",headers:{"Content-Type":"application/x-www-form-urlencoded"},body:`name=${enc(name)}`});await loadData();toast("Created")}catch(e){toast("Could not reach ESP32")}}
$("#addTodo").onclick=()=>createItem("todo");$("#addHabit").onclick=()=>createItem("habit");

function formatTime(s){
    let m=Math.floor(s/60);
    let sec=s%60;
    return String(m).padStart(2,"0")+":"+String(sec).padStart(2,"0");
}

function updateFocusUI(data){

    timerSeconds = data.remaining || 0;
    timerRunning = !!data.running;
    focusDurationSeconds = data.duration || 1500;

    $("#timer").textContent = formatTime(timerSeconds);
    $("#dashFocus").textContent = formatTime(timerSeconds);

    if(timerRunning){

        $("#timerStart").textContent = "Pause";
        $("#timerStatus").textContent = "Focusing…";

    }else if(timerSeconds === 0){

        $("#timerStart").textContent = "Start";
        $("#timerStatus").textContent = "Complete";

    }else if(timerSeconds === focusDurationSeconds){

        $("#timerStart").textContent = "Start";
        $("#timerStatus").textContent = "Ready";

    }else{

        $("#timerStart").textContent = "Start";
        $("#timerStatus").textContent = "Paused";

    }
}
async function loadFocus(){

    try{
        const data=await api("/api/focus");
        updateFocusUI(data);
    }catch(e){
        console.error("Focus sync error:",e);
    }
}

async function startFocus(){

    try{
        const data=await api("/api/focus/start",{
            method:"POST"
        });

        updateFocusUI(data);
    }catch(e){
        toast("Could not reach ESP32");
    }
}

async function pauseFocus(){

    try{
        const data=await api("/api/focus/pause",{
            method:"POST"
        });

        updateFocusUI(data);
    }catch(e){
        toast("Could not reach ESP32");
    }
}

async function resetFocus(){

    try{
        const data=await api("/api/focus/reset",{
            method:"POST"
        });

        updateFocusUI(data);
    }catch(e){
        toast("Could not reach ESP32");
    }
}

$("#timerStart").onclick=async()=>{

    if(timerRunning){
        await pauseFocus();
    }else{
        await startFocus();
    }

};

$("#timerReset").onclick=async()=>{
    await resetFocus();
};

$$(".preset-row button").forEach(b=>{

    b.onclick = async()=>{

        const minutes = Number(b.dataset.min);

        try{

            const data = await api("/api/focus/preset",{
                method:"POST",
                headers:{
                    "Content-Type":"application/x-www-form-urlencoded"
                },
                body:`minutes=${minutes}`
            });

            updateFocusUI(data);

            toast(`${minutes} min preset`);

        }catch(e){

            toast("Could not reach ESP32");

        }

    };

});

setInterval(loadFocus,1000);
function renderAlarms(){if(!alarms.length){$("#alarmList").innerHTML=empty("No alarms yet.");return}$("#alarmList").innerHTML=alarms.map((a,i)=>`<div class="item"><div class="item-main"><div class="item-title">${a.time}</div><div class="item-meta">${a.enabled?"Enabled":"Disabled"}</div></div><button class="secondary" onclick="toggleAlarm(${i})">${a.enabled?"ON":"OFF"}</button><button class="tiny" onclick="deleteAlarm(${i})">Delete</button></div>`).join("")}
$("#addAlarm").onclick=()=>{let time=prompt("Alarm time (HH:MM)","08:00");if(!time)return;alarms.push({time,enabled:true});localStorage.setItem("localAlarms",JSON.stringify(alarms));renderAlarms()};
function toggleAlarm(i){alarms[i].enabled=!alarms[i].enabled;localStorage.setItem("localAlarms",JSON.stringify(alarms));renderAlarms()}
function deleteAlarm(i){alarms.splice(i,1);localStorage.setItem("localAlarms",JSON.stringify(alarms));renderAlarms()}
function escapeHtml(s){return String(s).replace(/[&<>"']/g,m=>({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#039;"}[m]))}
setInterval(loadStatus,5000);
setInterval(loadData,10000);

loadFocus();
renderAlarms();
loadAll();
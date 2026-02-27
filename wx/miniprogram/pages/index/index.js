const db = wx.cloud.database();
const _ = db.command;

Page({
  data: {
    tasks: [],
    newTaskTitle: '',
    // 计时器相关状态
    timerRunning: false,
    activeTaskId: null,
    seconds: 0,
    timerDisplay: '00:00:00'
  },

  onLoad: function() {
    this.fetchTasks();
  },

  // 获取今日任务
  fetchTasks: function() {
    const today = new Date().toISOString().split('T')[0];
    db.collection('tasks').where({
      date: today
    }).orderBy('createdAt', 'asc').get().then(res => {
      this.setData({ tasks: res.data });
    });
  },

  // 切换计时状态
  toggleTimer: function(e) {
    const { id } = e.currentTarget.dataset;
    if (this.data.timerRunning) {
      if (this.data.activeTaskId === id) {
        this.stopTimer();
      } else {
        wx.showToast({ title: '请先停止当前计时', icon: 'none' });
      }
    } else {
      this.startTimer(id);
    }
  },

  startTimer: function(id) {
    this.setData({
      timerRunning: true,
      activeTaskId: id,
      seconds: 0,
      timerDisplay: '00:00:00'
    });

    this.timerInterval = setInterval(() => {
      let secs = this.data.seconds + 1;
      this.setData({
        seconds: secs,
        timerDisplay: this.formatTime(secs)
      });
    }, 1000);
    
    // 修正：使用微信标准震动接口
    wx.vibrateShort({ type: 'medium' });
  },

  stopTimer: function() {
    if (this.timerInterval) {
      clearInterval(this.timerInterval);
    }
    const { activeTaskId, seconds } = this.data;

    wx.showLoading({ title: '保存中...' });
    db.collection('tasks').doc(activeTaskId).update({
      data: {
        actualDuration: _.inc(seconds)
      }
    }).then(() => {
      wx.hideLoading();
      this.setData({ timerRunning: false, activeTaskId: null });
      this.fetchTasks();
    });
  },

  // 核心：标记任务完成
  markAsDone: function(e) {
    const { id } = e.currentTarget.dataset;
    
    // 如果该任务正在计时，强制要求先停止
    if (this.data.timerRunning && this.data.activeTaskId === id) {
      wx.showToast({ title: '请先停止计时', icon: 'none' });
      return;
    }

    wx.showLoading({ title: '更新状态...' });
    db.collection('tasks').doc(id).update({
      data: { status: 'done' }
    }).then(() => {
      wx.hideLoading();
      // 修正：将报错的 vibrateMedium 改为 vibrateShort
      wx.vibrateShort({ type: 'heavy' }); 
      this.fetchTasks();
    }).catch(err => {
      wx.hideLoading();
      wx.showToast({ title: '更新失败', icon: 'none' });
      console.error(err);
    });
  },

  formatTime: function(s) {
    const h = Math.floor(s/3600).toString().padStart(2, '0');
    const m = Math.floor((s%3600)/60).toString().padStart(2, '0');
    const sec = (s%60).toString().padStart(2, '0');
    return `${h}:${m}:${sec}`;
  },

  onInput: function(e) { this.setData({ newTaskTitle: e.detail.value }); },

  addTask: function() {
    if (!this.data.newTaskTitle || this.data.tasks.length >= 3) return;
    db.collection('tasks').add({
      data: {
        title: this.data.newTaskTitle,
        status: 'pending',
        actualDuration: 0,
        date: new Date().toISOString().split('T')[0],
        createdAt: db.serverDate()
      }
    }).then(() => {
      this.setData({ newTaskTitle: '' });
      this.fetchTasks();
    });
  }
});
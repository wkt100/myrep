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

    // 弹出心情评分弹窗
    wx.showModal({
      title: '完成心情',
      content: '你完成这个任务时的心情如何？',
      editable: true,
      placeholderText: '请输入1-5星（5星最好）',
      success: (res) => {
        if (res.confirm) {
          const mood = parseInt(res.content) || 3;
          const validMood = Math.max(1, Math.min(5, mood));

          wx.showLoading({ title: '更新状态...' });
          
          // 更新任务状态
          db.collection('tasks').doc(id).update({
            data: { 
              status: 'done',
              mood: validMood,
              completedAt: db.serverDate()
            }
          }).then(() => {
            wx.hideLoading();
            wx.vibrateShort({ type: 'heavy' });
            
            // 更新或创建 daily_summaries 记录
            this.updateDailySummary(validMood);
            
            this.fetchTasks();
          }).catch(err => {
            wx.hideLoading();
            wx.showToast({ title: '更新失败', icon: 'none' });
            console.error(err);
          });
        }
      }
    });
  },

  // 更新每日心情汇总
  updateDailySummary: function(mood) {
    const today = new Date().toISOString().split('T')[0];
    
    db.collection('daily_summaries').where({
      date: today
    }).get().then(res => {
      if (res.data.length > 0) {
        // 更新现有记录
        const summary = res.data[0];
        const newMoods = [...(summary.moods || []), mood];
        const avgMood = (newMoods.reduce((a, b) => a + b, 0) / newMoods.length).toFixed(1);
        
        db.collection('daily_summaries').doc(summary._id).update({
          data: {
            moods: newMoods,
            avgMood: parseFloat(avgMood),
            taskCount: _.inc(1),
            updatedAt: db.serverDate()
          }
        });
      } else {
        // 创建新记录
        db.collection('daily_summaries').add({
          data: {
            date: today,
            moods: [mood],
            avgMood: mood,
            taskCount: 1,
            createdAt: db.serverDate(),
            updatedAt: db.serverDate()
          }
        });
      }
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
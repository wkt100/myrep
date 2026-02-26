const db = wx.cloud.database();

Page({
  data: {
    newTaskTitle: '',
    newTaskStandard: '',
    tasks: [],
    loading: true
  },

  onLoad: function() {
    this.fetchTodayTasks();
  },

  // 从云数据库获取今日任务
  fetchTodayTasks: function() {
    const today = new Date().toISOString().split('T')[0];
    db.collection('tasks')
      .where({
        date: today
      })
      .orderBy('createdAt', 'asc')
      .get()
      .then(res => {
        this.setData({
          tasks: res.data,
          loading: false
        });
      });
  },

  // 输入框绑定
  onTitleInput: function(e) { this.setData({ newTaskTitle: e.detail.value }); },
  onStandardInput: function(e) { this.setData({ newTaskStandard: e.detail.value }); },

  // 添加任务（核心功能）
  addTask: function() {
    const { newTaskTitle, newTaskStandard, tasks } = this.data;
    
    if (tasks.length >= 3) {
      wx.showToast({ title: '今日已有三件事了', icon: 'none' });
      return;
    }
    if (!newTaskTitle || !newTaskStandard) {
      wx.showToast({ title: '请填写完整内容', icon: 'none' });
      return;
    }

    wx.showLoading({ title: '同步中...' });
    
    db.collection('tasks').add({
      data: {
        title: newTaskTitle,
        standard: newTaskStandard,
        status: 'pending',
        date: new Date().toISOString().split('T')[0],
        createdAt: db.serverDate(),
        actualDuration: 0
      }
    }).then(() => {
      wx.hideLoading();
      this.setData({ newTaskTitle: '', newTaskStandard: '' });
      this.fetchTodayTasks();
    });
  },

  // 更新任务状态（开始、完成、失败）
  updateStatus: function(e) {
    const { id, status } = e.currentTarget.dataset;
    wx.showLoading({ title: '更新中...' });
    
    db.collection('tasks').doc(id).update({
      data: { status: status }
    }).then(() => {
      wx.hideLoading();
      this.fetchTodayTasks();
    });
  }
});
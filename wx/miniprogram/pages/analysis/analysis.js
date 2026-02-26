const db = wx.cloud.database();

Page({
  data: {
    stats: {
      totalTasks: 0,
      completionRate: 0,
      topFailReason: '无',
      totalDays: 0
    }
  },

  onShow: function() {
    this.calculateStats();
  },

  calculateStats: function() {
    db.collection('tasks').get().then(res => {
      const tasks = res.data;
      if (tasks.length === 0) return;

      const doneTasks = tasks.filter(t => t.status === 'done');
      const failTasks = tasks.filter(t => t.status === 'fail');
      
      // 找出最频繁的失败原因
      const reasons = failTasks.map(t => t.failReason).filter(r => r);
      let commonReason = '无';
      if (reasons.length > 0) {
        const counts = reasons.reduce((a, b) => ({ ...a, [b]: (a[b] || 0) + 1 }), {});
        commonReason = Object.keys(counts).reduce((a, b) => counts[a] > counts[b] ? a : b);
      }

      // 计算独立的天数
      const days = [...new Set(tasks.map(t => t.date))].length;

      this.setData({
        stats: {
          totalTasks: tasks.length,
          completionRate: Math.round((doneTasks.length / tasks.length) * 100),
          topFailReason: commonReason,
          totalDays: days
        }
      });
    });
  }
});
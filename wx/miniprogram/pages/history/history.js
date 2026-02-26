const db = wx.cloud.database();

Page({
  data: {
    historyData: [],
    loading: true
  },

  onShow: function() {
    this.fetchHistory();
  },

  fetchHistory: function() {
    // 获取过去 30 天的数据进行展示
    db.collection('tasks')
      .orderBy('date', 'desc')
      .orderBy('createdAt', 'desc')
      .get()
      .then(res => {
        // 按日期对任务进行分组
        const groups = this.groupTasksByDate(res.data);
        this.setData({
          historyData: groups,
          loading: false
        });
      });
  },

  groupTasksByDate: function(tasks) {
    const groups = {};
    tasks.forEach(task => {
      if (!groups[task.date]) {
        groups[task.date] = {
          date: task.date,
          tasks: [],
          rate: 0
        };
      }
      groups[task.date].tasks.push(task);
    });

    // 计算每日完成率
    return Object.values(groups).map(group => {
      const doneCount = group.tasks.filter(t => t.status === 'done').length;
      group.rate = Math.round((doneCount / group.tasks.length) * 100);
      // 根据完成率设置颜色等级
      group.levelClass = group.rate >= 80 ? 'high' : (group.rate >= 50 ? 'mid' : 'low');
      return group;
    });
  }
});
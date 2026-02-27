const db = wx.cloud.database();

Page({
  data: {
    stats: {
      totalTasks: 0,
      completionRate: 0,
      topFailReason: '无',
      totalDays: 0
    },
    // 能量曲线数据
    moodData: [],
    // 时间块地图数据
    timeBlockData: {
      morning: { count: 0, label: '早晨 (6-12点)', percentage: 0 },
      afternoon: { count: 0, label: '下午 (12-18点)', percentage: 0 },
      evening: { count: 0, label: '晚上 (18-24点)', percentage: 0 },
      night: { count: 0, label: '深夜 (0-6点)', percentage: 0 }
    },
    bestTimeBlock: ''
  },

  onShow: function() {
    this.calculateStats();
    this.fetchMoodData();
    this.fetchTimeBlockData();
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
  },

  // 获取能量曲线数据（心情变化趋势）
  fetchMoodData: function() {
    db.collection('daily_summaries')
      .orderBy('date', 'asc')
      .limit(7)
      .get()
      .then(res => {
        const moodData = res.data.map(item => {
          const mood = item.avgMood || 0;
          let color;
          if (mood >= 4.5) color = '#52c41a'; // 优秀 - 绿色
          else if (mood >= 3.5) color = '#1890ff'; // 良好 - 蓝色
          else if (mood >= 2.5) color = '#faad14'; // 一般 - 橙色
          else if (mood >= 1.5) color = '#fa8c16'; // 较差 - 深橙色
          else color = '#f5222d'; // 很差 - 红色
          
          return {
            date: this.formatDate(item.date),
            avgMood: mood,
            moods: item.moods || [],
            color: color
          };
        });
        this.setData({ moodData });
      });
  },

  // 获取时间块地图数据
  fetchTimeBlockData: function() {
    db.collection('tasks')
      .where({ status: 'done' })
      .field({ completedAt: true })
      .get()
      .then(res => {
        const tasks = res.data;
        const timeBlocks = {
          morning: 0,   // 6-12
          afternoon: 0, // 12-18
          evening: 0,   // 18-24
          night: 0      // 0-6
        };

        tasks.forEach(task => {
          if (task.completedAt) {
            const hour = new Date(task.completedAt).getHours();
            if (hour >= 6 && hour < 12) {
              timeBlocks.morning++;
            } else if (hour >= 12 && hour < 18) {
              timeBlocks.afternoon++;
            } else if (hour >= 18 && hour < 24) {
              timeBlocks.evening++;
            } else {
              timeBlocks.night++;
            }
          }
        });

        const total = Object.values(timeBlocks).reduce((a, b) => a + b, 0);
        
        // 找出最佳时间块
        let bestTime = '';
        let maxCount = 0;
        const timeLabels = {
          morning: '早晨 (6-12点)',
          afternoon: '下午 (12-18点)',
          evening: '晚上 (18-24点)',
          night: '深夜 (0-6点)'
        };

        Object.keys(timeBlocks).forEach(key => {
          if (timeBlocks[key] > maxCount) {
            maxCount = timeBlocks[key];
            bestTime = timeLabels[key];
          }
        });

        this.setData({
          timeBlockData: {
            morning: { 
              count: timeBlocks.morning, 
              label: '早晨', 
              percentage: total ? Math.round((timeBlocks.morning / total) * 100) : 0,
              color: '#52c41a'
            },
            afternoon: { 
              count: timeBlocks.afternoon, 
              label: '下午', 
              percentage: total ? Math.round((timeBlocks.afternoon / total) * 100) : 0,
              color: '#1890ff'
            },
            evening: { 
              count: timeBlocks.evening, 
              label: '晚上', 
              percentage: total ? Math.round((timeBlocks.evening / total) * 100) : 0,
              color: '#722ed1'
            },
            night: { 
              count: timeBlocks.night, 
              label: '深夜', 
              percentage: total ? Math.round((timeBlocks.night / total) * 100) : 0,
              color: '#fa8c16'
            }
          },
          bestTimeBlock: maxCount > 0 ? bestTime : '暂无数据'
        });
      });
  },

  formatDate: function(dateStr) {
    const date = new Date(dateStr);
    const month = date.getMonth() + 1;
    const day = date.getDate();
    return `${month}/${day}`;
  }
});
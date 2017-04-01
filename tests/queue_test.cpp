#include <gtest/gtest.h>

TEST(QueueTest, NewQueue) {
    queue *q = new_queue(sizeof(int));
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(sizeof(int) + sizeof(queue_item), q->element_size);
}

TEST(QueueTest, PushPopFront) {
    queue *q = new_queue(sizeof(int));
    int *in = (int *)queue_push_front(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));
    ASSERT_EQ(1, queue_size(q));

    int *out = (int *)queue_pop_front(q);
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
    queue_free_item(out);
}

TEST(QueueTest, PushPopBack) {
    queue *q = new_queue(sizeof(int));
    int *in = (int *)queue_push_back(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));
    ASSERT_EQ(1, queue_size(q));

    int *out = (int *)queue_pop_back(q);
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
    queue_free_item(out);
}

TEST(QueueTest, PushPopManyFront) {
    queue *q = new_queue(sizeof(int));
    for(int i = 0; i < 100; i++) {
        int *in = (int *)queue_push_back(q);
        memcpy(in, &i, sizeof(int));
        ASSERT_EQ(i+1, queue_size(q));
    }

    for(int i = 0; i < 100; i++) {
        int *out = (int *)queue_pop_front(q);
        ASSERT_EQ(i, *out);
        queue_free_item(out);
    }
}

TEST(QueueTest, PushPopManyBack) {
    queue *q = new_queue(sizeof(int));
    for (int i = 0; i < 100; i++) {
        int *in = (int *)queue_push_front(q);
        memcpy(in, &i, sizeof(int));
        ASSERT_EQ(i+1, queue_size(q));
    }

    for(int i = 99; i >= 0; i--) {
        int *out = (int *)queue_pop_front(q);
        ASSERT_EQ(i, *out);
        queue_free_item(out);
    }
}
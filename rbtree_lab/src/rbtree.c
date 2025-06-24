#include "rbtree.h"
#include <stdlib.h>

/// @brief 레드블랙트리 생성 및 초기화
/// @return 초기화된 레드 블랙 트리의 포인터, 메모리 할당 실패 시 NULL
rbtree *new_rbtree(void)
{
  // 트리 구조체 동적 할당
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  if (t == NULL) // 메모리 할당 실패 시 NULL 리턴
    return NULL;

  // nil 노드 동적 할당
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  if (nil == NULL) // nil 메모리 할당 실패 시
  {
    free(t); // 트리 구조체 메모리 해제
    return NULL; // 메모리 할당 실패 시 NULL 반환
  }

  // nil은 항상 블랙
  nil->color = RBTREE_BLACK;
  // 초기화
  nil->left = nil;
  nil->right = nil;
  nil->parent = nil;

  t->nil = nil;
  t->root = nil;

  return t;
}

/// @brief 레드블렉트리 삽입 함수
/// @param t 삽입할 트리 포인터
/// @param key 삽입할 키 값
/// @return 삽입 성공 시 삽입한 노드 리턴, 중복 값이라면 삽입한 노드 리턴
node_t *rbtree_insert(rbtree *t, const key_t key) 
{
  // 삽입할 노드 초기화
  node_t *cur = (node_t*)calloc(1, sizeof(node_t));
  cur->color = RBTREE_RED;
  cur->key = key;
  cur->left = t->nil;
  cur->right = t->nil;
  cur->parent = t->nil;

  node_t *parent = t->nil;    // 삽입 위치의 부모 노드 저장 변수
  node_t *new_node = t->root; // 현재 탐색 중인 노드 

  while (new_node != t->nil) // nil 노드가 아닐때까지 반복
  {
    parent = new_node; // 현재 노드를 부모로 저장

    // if (key == new_node->key) // 키가 같다면
    // {
    //   free(cur); // 노드 메모리 해제
    //   return new_node; // 기존 노드 반환
    // }

    if (key < new_node->key) // 키가 작으면
      new_node = new_node->left; // 왼쪽으로 이동
    else // 키가 크면
      new_node = new_node->right; // 오른쪽으로 이동
  }
  
  cur->parent = parent; // 부모 노드 설정
  
  if (parent == t->nil) // 루트 노드가 없다면, 새 노드를 루트로
    t->root = cur;
  else if (key < parent->key) // 키가 부모보다 작으면
    parent->left = cur; // 부모의 왼쪽 노드 연결
  else // 키가 부모보다 크면
    parent->right = cur; // 부모의 오른쪽 노드 연결

  rbtree_insert_fixup(t, cur);
  return cur;
}

/// @brief 레드블랙트리 삽입 후 색상 및 밸런싱 함수
/// @param t 조정할 트리 포인터
/// @param cur 삽입된 노드
void rbtree_insert_fixup(rbtree *t, node_t *cur)
{
  node_t *uncle = t->nil;

  // 부모가 RED인 경우
  while (cur->parent->color == RBTREE_RED)
  {
    // case 1: 부모가 조부모의 왼쪽 자식일 경우
    if (cur->parent == cur->parent->parent->left)
    {
      uncle = cur->parent->parent->right; // 삼촌 노드는 조부모의 오른쪽 자식
      
      // case 1-1: 삼촌이 RED이면 색상 변경
      if (uncle->color == RBTREE_RED)
      {
        cur->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        cur = cur->parent->parent; // 조부모에서 다시 검사
      }
      // 삼촌이 BLACK인 경우
      else
      {
        // case 1-2: 삽입 노드가 오른쪽 자식이면 -> 왼쪽 회전으로 case 1-3으로 변환
        if (cur == cur->parent->right)
        {
          cur = cur->parent;
          left_rotate(t, cur);
        }

        // case 1-3: 삽입 노드가 왼쪽 자식이면 -> 재색칠 후 오른쪽 회전
        cur->parent->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        right_rotate(t, cur->parent->parent);
      }
    }
    // case 2: 부모가 조부모의 오른쪽 자식일 경우 (case 1을 대칭 처리)
    else
    {
      uncle = cur->parent->parent->left; // 삼촌 노드는 조부모의 왼쪽 자식

      // case 2-1: 삼촌이 RED -> 재색칠
      if (uncle->color == RBTREE_RED)
      {
        cur->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        cur = cur->parent->parent; // 조부모에서 다시 검사
      }
      // 삼촌이 BLACK인 경우
      else
      {
        // case 2-2: 삽입 노드가 왼쪽 자식 -> 오른쪽 회전으로 case 2-3으로 변환
        if (cur == cur->parent->left)
        {
          cur = cur->parent;
          right_rotate(t, cur);
        }
        
        // case 2-3: 삽입 노드가 오른쪽 자식이면 -> 재색칠 후 왼쪽 회전
        cur->parent->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        left_rotate(t, cur->parent->parent);
      }
    }
  }

  // 루트는 항상 BLACK이어야 하므로
  t->root->color = RBTREE_BLACK;
}

/// @brief 왼쪽 회전
/// @param t 회전할 트리 포인터
/// @param x 회전할 노드 값
void left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right; // y = x의 오른쪽 자식
  x->right = y->left;   // x의 오른쪽 자식을 y의 왼쪽 자식으로 연결

  if (y->left != t->nil) // y의 왼쪽 자식이 nil 노드가 아니라면
    y->left->parent = x; // 부모를 x로 연결
  
  y->parent = x->parent; // y의 부모를 x의 부모로 연결

  if (x->parent == t->nil) // x가 루트 노드였다면
    t->root = y;           // y가 루트
  else if (x == x->parent->left) // x가 부모의 왼쪽 자식이었다면
    x->parent->left = y;         // y를 왼쪽 자식으로 연결
  else                    // x가 부모의 오른쪽 자식이었다면
    x->parent->right = y; // y를 오른쪽 자식으로 연결

  y->left = x; // x를 y의 오른쪽 자식으로 연결
  x->parent = y; // x의 부모를 y로 설정
}

/// @brief 오른쪽 회전
/// @param t 회전할 트리 포인터
/// @param x 회전할 노드 값
void right_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->left;
  x->left = y->right;

  if (y->right != t->nil)
    y->right->parent = x;
    
  y->parent = x->parent;

  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;
  
  y->right = x;
  x->parent = y;
}

/// @brief 트리를 삭제하고 메모리 해제하는 함수
/// @param t 삭제할 트리 포인터
void delete_rbtree(rbtree *t) 
{
  delete_node(t, t->root); // 루트부터 시작
  free(t->nil); // nil 노드 메모라 해제
  free(t); // 트리 메모리 해제
}

/// @brief 트리의 모든 노드 후위순회하며 삭제하는 함수
/// @param t 삭제할 트리 포인터
/// @param node 현재 삭제할 노드
void delete_node(rbtree *t, node_t *node)
{
  if (node == t->nil) // 현재 노드가 nil이면 반환 (자식 노드가 없음)
    return;
  
  delete_node(t, node->left); 
  delete_node(t, node->right);

  free(node);
}

/// @brief 레드 블랙 트리의 key를 가진 노드를 찾는 함수
/// @param t 탐색할 레드 블랙 트리의 포인터
/// @param key 찾을 키
/// @return 해당 key를 가진 node의 포인터, 없을 시 NULL 반환
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *cur = t->root;

  while (cur != t->nil) // nil node가 아니면 반복
  {
    if (key < cur->key) // 키가 현재 노드의 키보다 작다면
      cur = cur->left;  // 왼쪽으로 이동
    else if (key > cur->key) // 키가 현재 노드의 키보다 크다면
      cur = cur->right; // 오른족으로 이동
    else // 키가 일치하면
      return cur; // 현재 노드 반환
  }
  
  return NULL; // 찾는 키가 없다면 NULL 반환
}

/// @brief 레드 블랙 트리 최소값을 가지는 노드를 반환
/// @param t 탐색할 트리의 포인터
/// @return 최소값을 가지는 노드의 포인터, 트리가 비어있으면 NULL
node_t *rbtree_min(const rbtree *t)
{
  node_t *cur = t->root;
  
  if (cur == t->nil)
    return NULL;

  while (cur->left != t->nil)
    cur = cur->left;
  
  return cur;
}

/// @brief 레드 블랙 트리의 서브트리의 최소값을 가지는 노드를 반환
/// @param t 탐색할 트리의 포인터
/// @param start 시작할 서브트리 노드
/// @return 서브트리의 최소값
node_t *rbtree_min_subtree(const rbtree *t, node_t *start)
{
  node_t *cur = start;

  while (cur->left != t->nil)
    cur = cur->left;
  
  return cur;
}

/// @brief 레드 블랙 트리 최댓값을 가지는 노드를 반환
/// @param t 탐색할 트리의 포인터
/// @return 최댓값을 가지는 노드의 포인터, 트리가 비어있으면 NULL
node_t *rbtree_max(const rbtree *t) 
{
  node_t *cur = t->root; // 현재 노드를 트리의 루트로 초기화

  if (cur == t->nil) // 트리가 비었다면 NULL 반환 
    return NULL;

  while (cur->right != t->nil) // 트리의 오른쪽이 nil node가 아닐때까지
    cur = cur->right; // 오른쪽 
  
  return cur; // 최댓값 노드 반환
}

/// @brief 트리에서 repalced_node를 substitude_node로 교체하는 함수
/// @param t 트리의 포인터
/// @param replaced_node 삭제될 노드
/// @param substitute_node 대체할 노드
void rbtree_transplant(rbtree *t, node_t *replaced_node, node_t *substitute_node)
{
  // replaced_node가 루트라면, substitute_node를 새로운 루트로 지정
  if (replaced_node->parent == t->nil)
    t->root = substitute_node;
  // replaced_node가 부모의 왼쪽 자식이면 substitute_node를 그 위치로 연결
  else if (replaced_node == replaced_node->parent->left)
    replaced_node->parent->left = substitute_node;
  // replaced_node가 부모의 오른쪽 자식이면 
  else
    replaced_node->parent->right = substitute_node;
  
  // substitute_node의 부모 포인터를 replaced_node의 부모로 설정
  substitute_node->parent = replaced_node->parent;
}

/// @brief 레드 블랙 트리 특정 노드 삭제 함수
/// @param t 트리 포인터
/// @param delete_node 삭제할 노드 포인터
/// @return 성공 시 0 반환
int rbtree_erase(rbtree *t, node_t *delete_node) 
{
  node_t *successor_node = delete_node; // 삭제될 노드 또는 그 위치를 대체할 후속자
  color_t orgin_color = successor_node->color; // 삭제될 노드의 원래 색깔
  node_t *fixup_node; // 삭제 후 수정 대상 노드
  
  // case 1: 삭제할 노드의 왼쪽 자식이 nil (오른쪽 자식만 있거나 자식이 없는 경우)
  if (delete_node->left == t->nil)
  {
    fixup_node = delete_node->right;                       // 삭제 후 fixup할 노드는 오른쪽 자식
    rbtree_transplant(t, delete_node, delete_node->right); // delete_node를 오른쪽 자식으로 대체
  }
  // case 2: 삭제할 노드의 오른쪽 자식이 nil (왼쪽 자식만 있는 경우)
  else if (delete_node->right == t->nil)
  {
    fixup_node = delete_node->left;                       // 삭제 후 fixup할 노드는 왼쪽 자식
    rbtree_transplant(t, delete_node, delete_node->left); // delete_node를 왼쪽 자식으로 대체
  }
  // case 3: 삭제할 노드가 자식이 둘 다 있는 경우
  else
  {
    // 삭제할 노드의 오른쪽 서브트리에서 최소값을 가지는 노드를 찾음 (후속자 설정)
    successor_node = rbtree_min_subtree(t, delete_node->right);
    orgin_color = successor_node->color;  // 후속자 색상 저장
    fixup_node = successor_node->right;   // 후속자가 오른쪽 자식만 가질 수 있으므로 fixup 대상은 그것

    // 후속자가 삭제할 노드의 바로 오른쪽 자식이 아닌 경우 (더 아래쪽에 위치)
    if (successor_node != delete_node->right)
    {
      // 후속자 노드를 그 오른쪽 자식으로 교체
      rbtree_transplant(t, successor_node, successor_node->right);
      // 후속자 노드의 오른쪽에 delete_node의 오른쪽 자식을 붙임
      successor_node->right = delete_node->right;
      successor_node->right->parent = successor_node;
    }
    // 후속자가 delete_node의 바로 오른쪽 자식인 경우
    else
    {
      // fixup_node의 부모를 후속자로 미리 설정
      fixup_node->parent = successor_node;
    }
    
    // delete_node를 후속자로 대체
    rbtree_transplant(t, delete_node, successor_node);
    // delete_node의 왼쪽 자식을 후속자에게 연결
    successor_node->left = delete_node->left;
    successor_node->left->parent = successor_node;
    // 삭제할 노드의 색을 후속자에게 전달 (레드 블랙 특성을 유지하기 위해)
    successor_node->color = delete_node->color;
  }

  // 삭제할 노드가 검은색이라면 트리의 속성을 깨뜨릴 수 있어 fixup
  if (orgin_color == RBTREE_BLACK)
    rbtree_delete_fixup(t, fixup_node);  

  // 삭제한 노드 메모리 해제
  free(delete_node);
  return 0;
}

/// @brief 레드 블랙 트리 삭제 후 밸런싱 함수
/// @param t 균형을 복구할 트리 포인터
/// @param fixup_node 삭제로 인해 불균형이 발생한 노드
void rbtree_delete_fixup(rbtree *t, node_t *fixup_node)
{
  node_t *sibling_node;

  // fixup_node가 루트가 아니고, fixup_node가 검정색일 때
  while (fixup_node != t->root && fixup_node->color == RBTREE_BLACK)
  {
    // fixup_n-node가 부모의 왼쪽 자식일 때
    if (fixup_node == fixup_node->parent->left)
    {
      sibling_node = fixup_node->parent->right; // fixup_node의 형제 노드
      
      // case 1: 형제 노드가 빨간색이면 재색칠 후 왼쪽 회전
      if (sibling_node->color == RBTREE_RED)
      {
        sibling_node->color = RBTREE_BLACK;
        fixup_node->parent->color = RBTREE_RED;
        left_rotate(t, fixup_node->parent);
        sibling_node = fixup_node->parent->right; // 형제 노드 갱신
      }

      // case 2: 형제 노드가 검정색이고 형제의 두 자식 모두 검정색인 경우
      if (sibling_node->left->color == RBTREE_BLACK && sibling_node->right->color == RBTREE_BLACK)
      {
        sibling_node->color = RBTREE_RED; // 형제를 빨간색으로 재색칠
        fixup_node = fixup_node->parent;  // fixup_node를 부모 노드로 올려 반복
      }
      else
      {
        // case 3: 형제 노드의 오른쪽 자식이 검적색일 때
        if (sibling_node->right->color == RBTREE_BLACK)
        {
          sibling_node->left->color = RBTREE_BLACK;
          sibling_node->color = RBTREE_RED;
          right_rotate(t, sibling_node);
          sibling_node = fixup_node->parent->right; // 형제 노드 갱신
        }
        
        // case 4: 형제 노드의 오른쪽 자식이 빨간색일 때
        sibling_node->color = fixup_node->parent->color;
        fixup_node->parent->color = RBTREE_BLACK;
        sibling_node->right->color = RBTREE_BLACK;
        left_rotate(t, fixup_node->parent);
        fixup_node = t->root; // 복구 끝내기 위해 루트로 이동
      }
    }
    // fixup_node가 부모의 오른쪽 자식일 때 (위의 case 1~4를 좌우 대칭 처리)
    else
    {
      sibling_node = fixup_node->parent->left;

      if (sibling_node->color == RBTREE_RED)
      {
        sibling_node->color = RBTREE_BLACK;
        fixup_node->parent->color = RBTREE_RED;
        right_rotate(t, fixup_node->parent);
        sibling_node = fixup_node->parent->left;
      }

      if (sibling_node->right->color == RBTREE_BLACK && sibling_node->left->color == RBTREE_BLACK)
      {
        sibling_node->color = RBTREE_RED;
        fixup_node = fixup_node->parent;
      }
      else
      {
        if (sibling_node->left->color == RBTREE_BLACK)
        {
          sibling_node->right->color = RBTREE_BLACK;
          sibling_node->color = RBTREE_RED;
          left_rotate(t, sibling_node);
          sibling_node = fixup_node->parent->left;
        }

        sibling_node->color = fixup_node->parent->color;
        fixup_node->parent->color = RBTREE_BLACK;
        sibling_node->left->color = RBTREE_BLACK;
        right_rotate(t, fixup_node->parent);
        fixup_node = t->root;
      }
    }
  }

  // fixup 작업이 끝난 후, 마지막 fixup 노드를 검정색으로 설정하여
  fixup_node->color = RBTREE_BLACK;
}

/// @brief 레드 블랙 트리 중위 순회하며 키 값을 배열에 저장하는 함수
/// @param t 대상 트리 포인터
/// @param arr 결과 저장할 배열 포인터
/// @param n 배열 크기
/// @param index 배열 내 현재 저장 위치에 대한 포인터
/// @return 성공 시 0
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{
  size_t index = 0; // 배열에 저장할 인덱스
  return rbtree_inorder(t, t->root, arr, n, &index);
}

/// @brief 레드 블랙 트리 중위 순회하며 키 값을 배열에 저장하는 재귀 함수
/// @param t 대상 트리 포인터
/// @param node 현재 방문 중인 노드 포인터
/// @param arr 결과 저장할 배열 포인터
/// @param n 배열 크기
/// @param index 배열 내 현재 저장 위치에 대한 포인터
/// @return 성공 시 0
int rbtree_inorder(const rbtree *t, node_t *node, key_t *arr, const size_t n, size_t *index)
{
  if (node == t->nil || *index >= n)
    return 0;
  
  // 왼쪽 서브트리 중위 순회
  rbtree_inorder(t, node->left, arr, n, index);

  // 현재 노드의 키를 배열에 저장 (범위 내에서만)
  if (*index < n)
  {
    arr[*index] = node->key;
    (*index)++;
  }

  // 오른쪽 서브트리 중위 순회
  rbtree_inorder(t, node->right, arr, n, index);

  return 0;
}